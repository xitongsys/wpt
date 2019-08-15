#include <iostream>
#include "config.h"
#include "Client.h"
#include "Server.h"
#pragma comment(lib, "Ws2_32.lib")

int main() {
	Config* config = new Config("config.txt");
	std::cout << config->to_string() << std::endl;

	if (config->role == "client") {
		Client* client = new Client(config);
		client->start();
	}
	else {
		Server* server = new Server(config);
		server->start();
	}
	return 0;
}