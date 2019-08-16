#pragma once
#include <string>
#include "json.hpp"
#include "route.h"
#include "ethernet/include/util.h"

class Config {
public:
	Config(string fname);

	string role;
	std::vector<int> client_tun_ports;
	std::vector<int> server_tun_ports;
	std::map<int,int> client2server;
	std::map<int,int> server2client;
	int direction;//0: client->server, 1: server->client
	string filter;

	string server_ip;
	int server_port;

	int if_index;

public:
	Route route;
	RouteItem *gateway;

	string to_string();
};
