#include "EventLoop.hpp"

extern int g_running;

#ifdef __APPLE__

EventLoop::~EventLoop() {
	if (_fd != -1) {
		close(_fd);
	}
}

EventLoop::EventLoop(std::vector<Server*>& s) :
	_servers(s),
	_fd(-1)
{
	if ((_fd = kqueue()) == -1) {
		throw std::runtime_error("kqueue failed");
	}

	size_t serverSize = s.size();
	for (size_t i = 0; i < serverSize; i++) {
		std::vector<int> vec = s[i]->getListenFd();
		size_t socketSize = vec.size();
		for (size_t j = 0; j < socketSize; j++) {
			_serverMap[vec[j]] = s[i];
			_addRead(vec[j]);
		}
	}
}

// register fd for read events
void	EventLoop::_addRead(int sock) {
	struct kevent event;
	EV_SET(&event, sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
	if (kevent(_fd, &event, 1, NULL, 0, NULL) == -1) {
		throw std::runtime_error("kevent failed at addRead");
	}
}

// register fd for write events
void	EventLoop::_addWrite(int sock) {
	struct kevent event;
	EV_SET(&event, sock, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, 0);
	if (kevent(_fd, &event, 1, NULL, 0, NULL) == -1) {
		throw std::runtime_error("kevent failed at addWrite");
	}
}

// deregister read fd
void	EventLoop::_removeRead(int sock) {
	struct kevent event;
	EV_SET(&event, sock, EVFILT_READ, EV_DELETE, 0, 0, 0);
	kevent(_fd, &event, 1, NULL, 0, NULL);
}

void	EventLoop::_removeWrite(int sock) {
	struct kevent event;
	EV_SET(&event, sock, EVFILT_WRITE, EV_DELETE, 0, 0, 0);
	kevent(_fd, &event, 1, NULL, 0, NULL);
}

#ifndef MAX_EVENTS
# define MAX_EVENTS 64
#endif

void	EventLoop::run() {
	struct kevent	events[MAX_EVENTS];

	while (g_running) {
		int	num;
		if ((num = kevent(_fd, NULL, 0, events, MAX_EVENTS, NULL)) == -1) {
			throw std::runtime_error("kevent failed at run");
		}
		for (int i = 0; i < num; i++) {
			if (events[i].flags & EV_ERROR || events[i].flags & EV_EOF) {
				_removeRead(events[i].ident);
				_removeWrite(events[i].ident);
				close(events[i].ident);
				if (_clientMap.find(events[i].ident) != _clientMap.end()) {
					delete _clientMap[events[i].ident];
					_clientMap.erase(events[i].ident);
				}
			}
			else if (_serverMap.find(events[i].ident) != _serverMap.end() && events[i].filter & EVFILT_READ) {
				int toAdd;
				if ((toAdd = accept(events[i].ident, NULL, NULL)) == -1) {
					continue;
				}
				if (fcntl(toAdd, F_SETFD, FD_CLOEXEC) == -1 || fcntl(toAdd, F_SETFL, O_NONBLOCK) == -1) {
					close(toAdd);
					throw std::runtime_error("fcntl failed");
				}
				_addRead(toAdd);
				_clientMap[toAdd] = new Client(toAdd, _serverMap[events[i].ident]);
			}
			else if (events[i].filter == EVFILT_READ) {
				if (_clientMap.find(events[i].ident) == _clientMap.end()) {
					continue;
				}
				_clientMap[events[i].ident]->read();
				if (_clientMap[events[i].ident]->getState() == C_SEND_R) {
					_addWrite(events[i].ident);
					_removeRead(events[i].ident);
					std::cout << "stuck here"<< std::endl;	// TODO: fix this bullshit
				}
				else if (_clientMap[events[i].ident]->getState() == C_ERROR) {
					_removeRead(events[i].ident);
					close(events[i].ident);
					delete _clientMap[events[i].ident];
					_clientMap.erase(events[i].ident);
				}
			}
			else if (events[i].filter == EVFILT_WRITE) {
				if (_clientMap.find(events[i].ident) == _clientMap.end()) {
					continue;
				}
				_clientMap[events[i].ident]->write();
				int stat = _clientMap[events[i].ident]->getState();
				if (stat == C_DONE || stat == C_ERROR) {
					_removeWrite(events[i].ident);
					close(events[i].ident);
					delete _clientMap[events[i].ident];
					_clientMap.erase(events[i].ident);
				}
			}
		}
	}
}
#endif
