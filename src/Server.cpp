#include "Server.hpp"

Server::Server(const ServerConfig& o) : ServerConfig(o) {
}

Server::~Server() {
	closeSockets();
}

void	Server::setup() {
	size_t i;
	for (i = 0; i < _listen.size(); i++) {
		s_socket sock;
		sock.fd = -1;
		std::memset(&sock.address, 0, sizeof(struct sockaddr_in));

		int opt = 1;
		sock.host = _listen[i].first;
		sock.port = _listen[i].second;
		sock.addrlen = sizeof(sock.address);

		if ((sock.fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
			throw std::runtime_error("socket failed");
		}
		if (fcntl(sock.fd, F_SETFD, FD_CLOEXEC) == -1 || fcntl(sock.fd, F_SETFL, O_NONBLOCK) == -1) {
			close(sock.fd);
			throw std::runtime_error("fcntl failed");
		}
		if (setsockopt(sock.fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
			close(sock.fd);
			throw std::runtime_error("setsockopt failed");
		}
		sock.address.sin_family = AF_INET;
		sock.address.sin_addr.s_addr = (sock.host.empty()) ? INADDR_ANY : inet_addr(sock.host.c_str());
		sock.address.sin_port = htons(sock.port);
		if (bind(sock.fd, (struct sockaddr*)&sock.address, sizeof(sock.address)) == -1) {
			close(sock.fd);
			throw std::runtime_error("bind failed");
		}
		if (listen(sock.fd, 128) == -1) {
			close(sock.fd);
			throw std::runtime_error("listen failed");
		}
		_sockets.push_back(sock);
	}
}

void	Server::closeSockets() {
	for (size_t j = 0; j < _sockets.size(); j++) {
		close(_sockets[j].fd);
	}
	_sockets.clear();
}

std::vector<int>	Server::getListenFd() const {
	std::vector<int> out;
	for (size_t i = 0; i < _sockets.size(); i++) {
		out.push_back(_sockets[i].fd);
	}
	return out;
}
