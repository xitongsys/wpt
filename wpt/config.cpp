#include "config.h"
#include <iostream>
#include <fstream>
#include <sstream>
using json = nlohmann::json;

Config::Config(string fname) {
	std::ifstream infile(fname);
	json j = json::parse(infile);

	role = j.value("role", "client");
	string server = j.value("server", "0.0.0.0:2222");
	int i = 0;
	while (i < server.size() && server[i] != ':')i++;
	server_ip = server.substr(0, i);
	server_port = atoi(server.substr(i + 1, (int)server.size() - i - 1).c_str());
	port = j.value("port", 3389);
	gateway = route.getRoute(str2ip(server_ip));
	if_index = gateway->ifIndex;
}

string Config::to_string() {
	char fmt[] = "role: %s\nserver: %s:%d\nport: %d\n";
	char buf[4096];
	sprintf_s(buf, 4096, fmt, role.c_str(), server_ip.c_str(), server_port, port);
	return buf;
}

