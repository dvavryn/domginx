#include "Config.hpp"

Config::Config(const std::string& f) {
	_rollout(f);
}

Config::Config(const char* f) {
	std::string filename = f;
	_rollout(filename);
}

void	Config::_rollout(const std::string& f) {
	std::ifstream in;
	in.open(f);
	if (in.is_open() == false) {
		throw std::runtime_error(f + " failed to open");
	}
	std::stringstream ss;
	ss << in.rdbuf();
	in.close();
	_raw = ss.str();
	_stripComments();
	_tokenize();
	_parse();
}


void	Config::_stripComments() {
	std::string out;
	size_t start = 0;
	size_t pos = 0;

	while (pos < _raw.size()) {
		if (_raw[pos] == '#') {
			out += _raw.substr(start, pos - start);
			while (pos < _raw.size() && _raw[pos] != '\n') {
				pos++;
			}
			start = pos;
			continue;
		}
		pos++;
	}
	out += _raw.substr(start);
	_raw = out;
}

std::string	Config::_getToken(size_t& p1) {
	size_t start = p1;
	if (std::strchr(";{}", _raw[p1]) != NULL) {
		p1++;
		return _raw.substr(start, 1);
	}
	if (_raw[p1] == '"') {
		p1++;
		while (p1 < _raw.size() && _raw[p1] != '"') {
			p1++;
		}
		if (p1 >= _raw.size() || _raw[p1] != '"') {
			throw std::runtime_error("syntax error: unclosed quotes");
		}
		p1++;
		return _raw.substr(start + 1, p1 - 2 - start);
	}
	else {
		while (p1 < _raw.size() && std::strchr("\t\n ;{}", _raw[p1]) == NULL) {
			p1++;
		}
		return _raw.substr(start, p1 - start);
	}
}

void	Config::_tokenize() {
	size_t	p1 = 0;

	while (p1 < _raw.size()) {
		while (p1 < _raw.size() && std::strchr("\t\n ", _raw[p1]) != NULL) {
			p1++;
		}
		if (p1 >= _raw.size()) {
			break;
		}
		std::string buf;
		buf = _getToken(p1);
		_tokens.push_back(buf);
	}
}

void	Config::_parse() {
	size_t	idx = 0;
	while (idx < _tokens.size()) {
		if (_tokens[idx] != "server") {
			throw std::runtime_error("syntax error: " + _tokens[idx]);
		}
		_serverConfigs.push_back(_parseServerBlock(++idx));
		idx++;
	}
}

ServerConfig	Config::_parseServerBlock(size_t& idx) {
	ServerConfig	out;
	size_t			size = _tokens.size();
	if (idx >= size || _tokens[idx] != "{") {
		throw std::runtime_error("syntax error: server");
	}
	while (++idx < size && _tokens[idx] != "}") {
		if (_tokens[idx] == "location") {
			out.addLocation(_parseLocationBlock(++idx));
			continue;
		}
		if (_tokens[idx] == "server_name") {
			idx++;
			while (idx < size && _tokens[idx] != ";") {
				out.addServerNames(_tokens[idx]);
				idx++;
			}
		}
		else if (_tokens[idx] == "listen") {
			idx++;
			if (idx >= size) {
				throw std::runtime_error("syntax error: " + _tokens[idx]);
			}
			size_t pos;
			char *c;
			std::strtol(_tokens[idx].c_str(), &c, 10);
			if ((pos = _tokens[idx].find(':')) != std::string::npos) {
				out.addListen(_tokens[idx].substr(0, pos), _tokens[idx].substr(pos + 1));
			}
			else if (*c == '\0') {
				out.addListen("", _tokens[idx]);
			}
			else {
				out.addListen(_tokens[idx], "80");
			}
			idx++;
		}
		else if (_tokens[idx] == "root") {
			idx++;
			if (idx >= size) {
				throw std::runtime_error("syntax error: " + _tokens[idx]);
			}
			out.setRoot(_tokens[idx++]);
		}
		else if (_tokens[idx] == "index") {
			idx++;
			if (idx + 1 >= size) {
				throw std::runtime_error("syntax error: " + _tokens[idx]);
			}
			out.setIndex(_tokens[idx++]);

		}
		else if (_tokens[idx] == "error_page") {
			idx++;
			if (idx + 1 >= size) {
				throw std::runtime_error("syntax error: " + _tokens[idx]);
			}
			out.addErrorPage(_tokens[idx], _tokens[idx + 1]);
			idx += 2;
		}
		else if (_tokens[idx] == "client_max_body_size") {
			idx++;
			if (idx >= size) {
				throw std::runtime_error("syntax error: " + _tokens[idx]);
			}
			out.setClientMaxBodySize(_tokens[idx++]);
		}
		else if (_tokens[idx] == "keepalive_timeout") {
			idx++;
			if (idx >= size) {
				throw std::runtime_error("syntax error: " + _tokens[idx]);
			}
			out.setKeepaliveTimeout(_tokens[idx++]);
		}
		else if (_tokens[idx] == "send_timeout") {
			idx++;
			if (idx >= size) {
				throw std::runtime_error("syntax error: " + _tokens[idx]);
			}
			out.setSendTimeout(_tokens[idx++]);
		}
		else if (_tokens[idx] == "access_log") {
			idx++;
			if (idx >= size) {
				throw std::runtime_error("syntax error: " + _tokens[idx]);
			}
			out.setAccessLog(_tokens[idx++]);
		}
		else if (_tokens[idx] == "error_log") {
			idx++;
			if (idx >= size) {
				throw std::runtime_error("syntax error: " + _tokens[idx]);
			}
			out.setErrorLog(_tokens[idx++]);
		}
		else {
			throw std::runtime_error("syntax error: unknown token: '" + _tokens[idx] + "'");
		}
		if (idx >= size || _tokens[idx] != ";") {
			throw std::runtime_error("syntax error: missing delimiter");
		}
	}
	if (idx >= size || _tokens[idx] != "}") {
		throw std::runtime_error("syntax error: server");
	}
	return out;
}

ServerConfig::Location	Config::_parseLocationBlock(size_t& idx) {
	ServerConfig::Location out;
	size_t size;
	size = _tokens.size();
	if (idx >= size || _tokens[idx][0] != '/') {
		throw std::runtime_error("syntax error: location");
	}
	out.path = _tokens[idx++];
	if (idx >= size || _tokens[idx] != "{") {
		throw std::runtime_error("syntax error: location");
	}

	while (++idx < size && _tokens[idx] != "}") {
		if (_tokens[idx] == "root") {
			idx++;
			if (idx >= size) {
				throw std::runtime_error("syntax error: " + _tokens[idx]);
			}
			out.root = _tokens[idx++];
		}
		else if (_tokens[idx] == "alias") {
			idx++;
			if (idx >= size) {
				throw std::runtime_error("syntax error: " + _tokens[idx]);
			}
			out.alias = _tokens[idx++];
		}
		else if (_tokens[idx] == "index") {
			idx++;
			if (idx >= size) {
				throw std::runtime_error("syntax error: " + _tokens[idx]);
			}
			out.index = _tokens[idx++];
		}
		else if (_tokens[idx] == "try_files") {
			idx++;
			while (idx < size && _tokens[idx] != ";") {
				out.tryFiles.push_back(_tokens[idx++]);
			}
		}
		else if (_tokens[idx] == "cgi_pass") {
			idx++;
			if (idx >= size) {
				throw std::runtime_error("syntax error: " + _tokens[idx]);
			}
			out.cgiPass = _tokens[idx++];
		}
		else if (_tokens[idx] == "upload_store") {
			idx++;
			if (idx >= size) {
				throw std::runtime_error("syntax error: " + _tokens[idx]);
			}
			out.uploadStore = _tokens[idx++];
		}
		else if (_tokens[idx] == "allow_methods") {
			idx++;
			while (idx < size && _tokens[idx] != ";") {
				out.allowedMethods.push_back(_tokens[idx++]);
			}
		}
		else if (_tokens[idx] == "autoindex") {
			idx++;
			if (idx >= size) {
				throw std::runtime_error("syntax error: " + _tokens[idx]);
			}
			out.autoIndex = (_tokens[idx++] == "on") ? true : false;
		}
		else if (_tokens[idx] == "client_max_body_size") {
			idx++;
			if (idx >= size) {
				throw std::runtime_error("syntax error: " + _tokens[idx]);
			}
			out.clientMaxBodySize = ServerConfig::calcClientMaxBodySize(_tokens[idx++]);
		}
		else if (_tokens[idx] == "return") {
			idx++;
			if (idx >= size) {
				throw std::runtime_error("syntax error: " + _tokens[idx]);
			}
			out._return.first = std::strtol(_tokens[idx++].c_str(), NULL, 10);
			if (idx < size && _tokens[idx] != ";") {
				out._return.second = _tokens[idx++];
			}
		}
		else {
			throw std::runtime_error("syntax error: unknown token");
		}
		if (idx >= size || _tokens[idx] != ";") {
			throw std::runtime_error("syntax error: " + _tokens[idx]);
		}
	}
	if (idx >= size || _tokens[idx] != "}") {
		throw std::runtime_error("syntax error: location");
	}
	return out;
}

const std::vector<ServerConfig>&	Config::getServerConfigs() const {
	return _serverConfigs;
}


Config::~Config() {
}
