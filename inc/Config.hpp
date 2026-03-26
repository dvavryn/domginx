#pragma once

#include "main.hpp"
#include "ServerConfig.hpp"
class Config {
private:
	std::vector<ServerConfig>	_serverConfigs;
	std::string					_raw;

	std::vector<std::string>	_tokens;

	void					_rollout(const std::string&);


	void					_stripComments();
	void					_tokenize();
	void					_parse();
	ServerConfig			_parseServerBlock(size_t&);
	ServerConfig::Location	_parseLocationBlock(size_t&);

	std::string	_getToken(size_t&);
	Config(const Config&);
	Config& operator=(const Config&);
public:
	Config(const std::string&);
	Config(const char*);
	~Config();

	const std::vector<ServerConfig>&	getServerConfigs() const;
};
