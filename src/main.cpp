#include "main.hpp"
#include "Config.hpp"
#include "Server.hpp"
#include "EventLoop.hpp"

volatile int	g_running = 1;

int main(int argc, char **argv) {
	std::string file;
	std::vector<Server*> servers;

	file = (argc == 1) ? "./configs/default.conf" : argv[1];
	if (config(file, servers) == false) {
		return cleanupServers(servers, 1);
	}
	if (rollout(servers) == false) {
		return cleanupServers(servers, 2);
	}
	if (run(servers) == false) {
		return cleanupServers(servers, 3);
	}
	return cleanupServers(servers, 0);
}

bool	run(std::vector<Server*>& s) {
	try {
		EventLoop loop(s);
		loop.run();
	}
	catch (std::exception& e) {
		std::cerr << "DOMGINX: " << e.what() << std::endl;
		return false;
	}
	return true;
}

bool	rollout(std::vector<Server*>& s) {
	try {
		for (size_t i = 0; i < s.size(); i++) {
			s[i]->setup();
		}
	}
	catch (std::exception& e) {
		std::cerr << "DOMGINX: " << e.what() << std::endl;
		return false;
	}
	return true;
}

bool	config(const std::string& f, std::vector<Server*>& s) {
	try {
		Config conf(f);
		const std::vector<ServerConfig>& ref = conf.getServerConfigs();
		for (size_t i = 0; i < ref.size(); i++) {
			s.push_back(new Server(ref[i]));
		}
	}
	catch (std::exception& e) {
		std::cerr << "DOMGINX: " << e.what() << std::endl;
		return false;
	}
	return true;
}

int	cleanupServers(std::vector<Server*>& s, int ret) {
	for (size_t i = 0; i < s.size(); i++) {
		if (s[i] != NULL) {
			delete s[i];
		}
	}
	if (ret != 0) {
		std::cerr << "DOMGINX: an error has occured | code ["
				  << ret << "]" << std::endl;
	}
	return ret;
}
