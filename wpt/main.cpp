#include <iostream>
#include "config.h"
#include "Client.h"
#include "Server.h"
using namespace std;
#pragma comment(lib, "Ws2_32.lib")

int main() {
	Config* config = new Config("config.txt");
	cout << config->to_string() << endl;

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