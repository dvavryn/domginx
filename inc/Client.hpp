#pragma once

#include "main.hpp"
#include "Server.hpp"

enum state {
	C_READ_H,	// client	read	headers
	C_PARS_H,	// client	parse	headers
	C_READ_B,	// client	read	body
	C_CREA_R,	// client	create	response
	C_SEND_R,	// client	send	response
	C_DONE,		// client	done
	C_ERROR		// client	error
};

class Client {
private:
	state								_state;
	int									_fd;
	const ServerConfig*					_serverConfig;
	std::string							_method;
	std::string							_httpVersion;
	std::string							_uri;
	std::map<std::string, std::string>	_headers;
	std::string							_body;

	std::string							_readBuffer;
	std::string							_writeBuffer;
	size_t								_bytesSent;

	time_t								_lastActivity;


	void	_readHeaders();
	void	_parseHeaders();
	void	_readBody();
	void	_createResponse();

	bool	_extractFirstLine(size_t);
	void	_splitHeader(size_t, size_t);

	Client(const Client&);
	Client& operator=(const Client&);
public:
	Client(int, const ServerConfig*);
	~Client();

	void	read();
	void	write();

	int	getState();
};