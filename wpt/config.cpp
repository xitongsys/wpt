#include "config.h"
#include <iostream>
#include <fstream>
#include <sstream>
using json = nlohmann::json;

Config::Config(string fname) {
	std::ifstream infile(fname);
	json j = json::parse(infile);

	role = j.value("role", "client");
	server = j.value("server", "0.0.0.0");
	port = atoi(j.value("port", "3389").c_str());
	udp_port = atoi(j.value("udp_port", "2222").c_str());
	gateway = route.getRoute(str2ip(server));
	if_index = gateway->ifIndex;
}

string Config::to_string() {
	char fmt[] = "role: %s\nserver: %s\nport: %d\nudp_port: %d\n";
	char buf[4096];
	sprintf_s(buf, 4096, fmt, role.c_str(), server.c_str(), port, udp_port);
	return buf;
}

