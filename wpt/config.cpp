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

	auto cports = j["client_tun_ports"];
	for (auto it = cports.begin(); it != cports.end(); it++) {
		client_tun_ports.push_back(*it);
	}

	auto sports = j["server_tun_ports"];
	for (auto it = sports.begin(); it != sports.end(); it++) {
		server_tun_ports.push_back(*it);
	}

	client2server.clear(); 
	server2client.clear();
	for (int i = 0; i < client_tun_ports.size() && i < server_tun_ports.size(); i++) {
		int c = client_tun_ports[i], s = server_tun_ports[i];
		client2server[c] = s;
		server2client[s] = c;
	}

	direction = j.value("direction", 0);

	gateway = route.getRoute(str2ip(server_ip));
	if_index = gateway->ifIndex;

	if (role == "client") {
		filter = "!impostor and ( ";
		for (int i = 0; i < client_tun_ports.size(); i++) {
			int p = client_tun_ports[i];
			if (i > 0) filter += " or ";
			filter += "localPort == " + std::to_string(p) + " ";
		}
		filter += ")";
	}
	else {
		filter = "!impostor and ( ";
		for (int i = 0; i < server_tun_ports.size(); i++) {
			int p = server_tun_ports[i];
			if (i > 0) filter += " or ";
			filter += "localPort == " + std::to_string(p) + " ";
		}
		filter += ")";
	}
}

string Config::to_string() {
	char fmt[] = "role: %s\nserver: %s:%d\nclient_tun_ports: %s\nserver_tun_ports: %s\nfilter: %s\n";
	char buf[4096];
	string cports;
	for (int i = 0; i < client_tun_ports.size(); i++) {
		if (i > 0) cports += ", ";
		cports += std::to_string(client_tun_ports[i]);
	}
	string sports;
	for (int i = 0; i < server_tun_ports.size(); i++) {
		if (i > 0) sports += ", ";
		sports += std::to_string(server_tun_ports[i]);
	}
	sprintf_s(buf, 4096, fmt, role.c_str(), server_ip.c_str(), server_port, cports.c_str(), sports.c_str(), filter.c_str());
	return buf;
}

