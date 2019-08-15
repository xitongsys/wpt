#pragma once

#include <string>
#include "json.hpp"
#include "route.h"
#include "ethernet/include/util.h"
using namespace std;

class Config {
public:
	Config(string fname);

	string role;
	int port;
	int udp_port;
	string server;

	int if_index;

public:
	Route route;
	RouteItem *gateway;

	string to_string();
};
