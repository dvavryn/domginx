#include "Client.hpp"

Client::Client(int fd, const ServerConfig* s) :
	_state(C_READ_H),
	_fd(fd),
	_serverConfig(s),
	_bytesSent(0),
	_lastActivity(time(NULL))
{
}

Client::~Client() {
}

void	Client::write() {
	ssize_t bsent;

	if (_writeBuffer.size() == 0) {
		_state = C_DONE;
		return;
	}
	while ((bsent = send(_fd, &_writeBuffer[_bytesSent], 4096, 0)) > 0) {
		_bytesSent += bsent;
		if (_bytesSent >= _writeBuffer.size()) {
			_state = C_DONE;
			return;
		}
	}
	if (bsent == -1) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			return;
		}
		_state = C_ERROR;
	}
	else if (bsent == 0) {
		_state = C_ERROR;
	}
}

void	Client::_createResponse() {
	_writeBuffer += "HTTP/1.1 200 OK\r\n";
	_writeBuffer += "Content-Type: text/html; charset=UTF-8\r\n";
	_writeBuffer += "Content-Length: 52\r\n";
	_writeBuffer += "Connection: close\r\n";
	_writeBuffer += "\r\n";
	_writeBuffer += "<html><body><h1>It Works for now</h1></body></html>";
	_state = C_SEND_R;
}

void	Client::read() {
	bool	progressing = true;
	while (progressing) {
		state before = _state;
		switch (_state) {
		case C_READ_H:	_readHeaders();		break;
		case C_PARS_H:	_parseHeaders();	break;
		case C_READ_B:	_readBody();		break;
		case C_CREA_R:	_createResponse();	break;
		default: return;
		}
		progressing = (_state != before && _state != C_ERROR && _state != C_SEND_R);
	}
}

void	Client::_readHeaders() {
	ssize_t	bread;
	char buffer[4096];
	
	while ((bread = recv(_fd, buffer, 4096, 0)) > 0) {
		_readBuffer.append(buffer, bread);
		if (_readBuffer.find("\r\n\r\n", _readBuffer.size() - bread) != std::string::npos) {
			_state = C_PARS_H;
			return;
		}
	}
	if (bread == -1) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			return;
		}
		_state = C_ERROR;
	}
	else if (bread == 0) {
		_state = C_ERROR;
	}
}

void	Client::_splitHeader(size_t start, size_t end) {
	std::string key;
	std::string val;
	size_t bound;
	size_t i;
	if ((bound = _readBuffer.find(':', start)) == std::string::npos || bound >= end ) {
		_state = C_ERROR;
		return;
	}
	for (i = bound + 1; i < end && std::isspace(_readBuffer[i]); i++);

	key = _readBuffer.substr(start, bound - start);
	val = _readBuffer.substr(i, end - i - 1);
	_headers[key] = val;
}

void	Client::_parseHeaders() {
	size_t	pos = 0;
	size_t	start;
	size_t	end;

	if ((end = _readBuffer.find("\r\n\r\n")) == std::string::npos) {
		_state = C_ERROR;
		return;
	}
	if ((pos = _readBuffer.find("\r\n")) == end) {
		_state = C_ERROR;
		return;
	}
	if (_extractFirstLine(pos) == false) {
		_state = C_ERROR;
		return;
	}
	pos += 2;
	start = pos;
	while ((pos = _readBuffer.find("\r\n", start)) < end && _state != C_ERROR) {
		_splitHeader(start, pos);
		start = pos + 2;
	}
	_readBuffer.erase(0, end + 4);
	if (_headers.find("Content-Length") != _headers.end()) {
		_state = C_READ_B;
	}
	else {
		_state = C_CREA_R;
	}
}

void	Client::_readBody() {
	ssize_t bread;
	char buffer[4096];
	size_t contLen;
	
	if (_headers.find("Content-Length") == _headers.end()) {
		_state = C_CREA_R;
		_readBuffer.clear();
		return ;
	}
	contLen = std::strtol(_headers["Content-Length"].c_str(), NULL, 10);
	while ((bread = recv(_fd, buffer, 4096, 0)) > 0) {
		_readBuffer.append(buffer, bread);
		if (contLen <= _readBuffer.size()) {
			_state = C_CREA_R;
			_body = _readBuffer;
			_readBuffer.clear();
			return;
		}
	}
	if (bread == -1) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			return ;
		}
		_state = C_ERROR;
	}
	else if (bread == 0) {
		_state = C_ERROR;
	}
}

bool	Client::_extractFirstLine(size_t end) {
	size_t	start = 0;
	size_t	pos = 0;

	size_t	count = 0;
	std::string	buf[3];
	while (pos < end) {
		while (pos < end && std::isspace(_readBuffer[pos]) == false) {
			pos++;
		}
		buf[count++] = _readBuffer.substr(start, pos - start);
		while (pos < end && std::isspace(_readBuffer[pos])) {
			pos++;
		}
		if (count == 3) {
			break;
		}
		start = pos;
	}
	if (count != 3 || pos != end) {
		return false;
	}
	_method = buf[0];
	_uri = buf[1];
	_httpVersion = buf[2];
	return true;
}

int	Client::getState() {
	return _state;
}
