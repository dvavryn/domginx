#pragma once

#include "main.hpp"
#include "ServerConfig.hpp"

class Server : public ServerConfig {
private:
	struct s_socket {
		std::string			host;
		int					fd;
		int					port;
		struct sockaddr_in	address;
		socklen_t			addrlen;
	};

	std::vector<s_socket>			_sockets;
	Server(const Server&);
	Server& operator=(const Server&);
public:
	Server(const ServerConfig&);
	~Server();

	void				setup();
	std::vector<int>	getListenFd() const;
	void				closeSockets();
};