#pragma once
#include <string>
#include "json.hpp"
#include "route.h"
#include "ethernet/include/util.h"

class Config {
public:
	Config(string fname);

	string role;
	int port;
	string server_ip;
	int server_port;

	int if_index;

public:
	Route route;
	RouteItem *gateway;

	string to_string();
};
