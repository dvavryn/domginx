#pragma once

#include <exception>
#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctime>

#define MAX_EVENTS 64

#ifdef __APPLE__
# include <sys/types.h>
# include <sys/event.h>
# include <sys/time.h>
#endif

class Server;
bool	config(const std::string&, std::vector<Server*>&);
bool	rollout(std::vector<Server*>&);
bool	run(std::vector<Server*>&);
int		cleanupServers(std::vector<Server*>&, int);
