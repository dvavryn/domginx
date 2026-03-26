#include "ServerConfig.hpp"

ServerConfig::ServerConfig() :
	_clientMaxBodySize(1024*1024),
	_keepaliveTimeout(75),
	_sendTimeout(60)
{
}

ServerConfig::ServerConfig(const ServerConfig& o) :
	_listen(o._listen),
	_serverNames(o._serverNames),
	_root(o._root),
	_index(o._index),
	_errorPages(o._errorPages),
	_clientMaxBodySize(o._clientMaxBodySize),
	_locations(o._locations),
	_keepaliveTimeout(o._keepaliveTimeout),
	_sendTimeout(o._sendTimeout),
	_accessLog(o._accessLog),
	_errorLog(o._errorLog)
{
}

ServerConfig& ServerConfig::operator=(const ServerConfig& o) {
	_listen = o._listen;
	_serverNames = o._serverNames;
	_root = o._root;
	_index = o._index;
	_errorPages = o._errorPages;
	_clientMaxBodySize = o._clientMaxBodySize;
	_locations = o._locations;
	_keepaliveTimeout = o._keepaliveTimeout;
	_sendTimeout = o._sendTimeout;
	_accessLog = o._accessLog;
	_errorLog = o._errorLog;
	return *this;
}

ServerConfig::~ServerConfig() {
}

void	ServerConfig::addListen(const std::string& host, const std::string& port) {
	long num;
	num = std::strtol(port.c_str(), NULL, 10);
	_listen.push_back(std::pair<std::string, int>(host, static_cast<int>(num)));
}

void	ServerConfig::addServerNames(const std::string& name) {
	_serverNames.push_back(name);
}

void	ServerConfig::setRoot(const std::string& path) {
	_root = path;
}

void	ServerConfig::setIndex(const std::string& path) {
	_index = path;
}

void	ServerConfig::addErrorPage(const std::string& err, const std::string& path) {
	long num;
	num = std::strtol(err.c_str(), NULL, 10);
	_errorPages[static_cast<int>(num)] = path;
}

void	ServerConfig::setClientMaxBodySize(const std::string& size) {
	_clientMaxBodySize = calcClientMaxBodySize(size);
}

void	ServerConfig::addLocation(const Location& loc) {
	_locations.push_back(loc);
}

void	ServerConfig::setKeepaliveTimeout(const std::string& sec) {
	long num;
	num = std::strtol(sec.c_str(), NULL, 10);
	_keepaliveTimeout = static_cast<size_t>(num);
}

void	ServerConfig::setSendTimeout(const std::string& sec) {
	long num;
	num = std::strtol(sec.c_str(), NULL, 10);
	_sendTimeout = static_cast<size_t>(num);
}

void	ServerConfig::setAccessLog(const std::string& path) {
	_accessLog = path;
}

void	ServerConfig::setErrorLog(const std::string& path) {
	_errorLog = path;
}


size_t ServerConfig::calcClientMaxBodySize(const std::string& size) {
	long num;
	char *c;
	num = std::strtol(size.c_str(), &c, 10);
	if (*c == 'k' || *c == 'K') {
		num *= 1024;
	}
	else if (*c == 'm' || *c == 'M') {
		num *= 1024 * 1024;
	}
	return static_cast<int>(num);
}

const std::string&	ServerConfig::getRoot() const {
	return _root;
}

const std::vector<ServerConfig::Location>&	ServerConfig::getLocations() const {
	return _locations;
}

const std::map<int, std::string>&	ServerConfig::getErrorPages() const {
	return _errorPages;
}

size_t	ServerConfig::getClientMaxBodySize() const {
	return _clientMaxBodySize;
}

const std::string&	ServerConfig::getIndex() const {
	return _index;
}

const std::vector<std::string>&	ServerConfig::getServerNames() const {
	return _serverNames;
}

const std::vector<std::pair<std::string, int> >	ServerConfig::getListen() const {
	return _listen;
}

size_t	ServerConfig::getKeepaliveTimeout() const {
	return _keepaliveTimeout;
}

size_t	ServerConfig::getSendTimeout() const {
	return _sendTimeout;
}

const std::string&	ServerConfig::getAccessLog() const {
	return _accessLog;
}

const std::string&	ServerConfig::getErrorLog() const {
	return _errorLog;
}
