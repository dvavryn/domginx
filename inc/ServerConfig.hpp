#pragma once

#include "main.hpp"

class ServerConfig {
public:
	struct Location;
protected:
	std::vector<std::pair<std::string, int> >	_listen;
	std::vector<std::string>					_serverNames;
	std::string									_root;
	std::string									_index;
	std::map<int, std::string>					_errorPages;
	size_t										_clientMaxBodySize;
	std::vector<Location>						_locations;
	size_t										_keepaliveTimeout;
	size_t										_sendTimeout;
	std::string									_accessLog;
	std::string									_errorLog;

public:
	ServerConfig();
	ServerConfig(const ServerConfig&);
	ServerConfig& operator=(const ServerConfig&);
	virtual ~ServerConfig();

	void	addListen(const std::string&, const std::string&);
	void	addServerNames(const std::string&);
	void	setRoot(const std::string&);
	void	setIndex(const std::string&);
	void	addErrorPage(const std::string&, const std::string&);
	void	setClientMaxBodySize(const std::string&);
	void	addLocation(const Location&);
	void	setKeepaliveTimeout(const std::string&);
	void	setSendTimeout(const std::string&);
	void	setAccessLog(const std::string&);
	void	setErrorLog(const std::string&);

	//	for client
	const std::string&								getRoot() const;
	const std::vector<Location>&					getLocations() const;
	const std::map<int, std::string>&				getErrorPages() const;
	size_t											getClientMaxBodySize() const;
	const std::string&								getIndex() const;
	const std::vector<std::string>&					getServerNames() const;
	const std::vector<std::pair<std::string, int> >	getListen() const;

	//	for event loop
	size_t	getKeepaliveTimeout() const;
	size_t	getSendTimeout() const;

	//	for logger
	const std::string&	getAccessLog() const;
	const std::string&	getErrorLog() const;

	static size_t calcClientMaxBodySize(const std::string&);
};

struct ServerConfig::Location {
	std::string					path;
	std::string 				root;
	std::vector<std::string>	allowedMethods;
	bool						autoIndex;
	std::string					index;
	std::pair<int, std::string>	_return;
	std::string					cgiPass;
	std::string 				uploadStore;
	std::string					alias;
	std::vector<std::string> 	tryFiles;
	size_t						clientMaxBodySize;
	Location() : autoIndex(false), clientMaxBodySize(0) {};
};