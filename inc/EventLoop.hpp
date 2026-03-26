#pragma once

#include "main.hpp"
#include "Server.hpp"
#include "Client.hpp"

class EventLoop {
private:
	struct s_events {
		int		fd;
		bool	readable;
		bool	writeable;
	};
	std::vector<Server*>&			_servers;
	std::map<int, Server*>			_serverMap;
	std::map<int, Client*>			_clientMap;
	int								_fd;

	std::vector<s_events>			_events;

	void	_addRead(int);
	void	_addWrite(int);
	void	_removeRead(int);
	void	_removeWrite(int);
	EventLoop(const EventLoop&);
	EventLoop& operator=(const EventLoop&);
public:
	EventLoop(std::vector<Server*>&);
	~EventLoop();

	void	run();
};