#include "Server.h"
#include <WinSock2.h>

Server::Server(Config* config) {
	this->config = config;
	this->tun = new Tun(config);
}

Server::~Server() {}

bool Server::start() {
	if (!tun->start()) return false;

	WORD socketVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(socketVersion, &wsaData) != 0) {
		return false;
	}

	int len = sizeof(server_sk_info);
	memset(&server_sk_info, 0, len);
	server_sk_info.sin_family = AF_INET;
	server_sk_info.sin_port = htons(config->server_port);
	server_sk_info.sin_addr.s_addr = INADDR_ANY;

	sk = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sk == SOCKET_ERROR) {
		return false;
	}
	bind(sk, (sockaddr*)& server_sk_info, sizeof(server_sk_info));

	std::thread send_thread(&Server::send, this);
	std::thread recv_thread(&Server::recv, this);

	send_thread.join();
	recv_thread.join();
}

void Server::send() {
	while (true) {
		vector<uint8_t> data = tun->read();
		if (data.size() > 0 && data.size() < BUFFSIZE) {
			for (int i = 0; i < data.size(); i++) {
				send_buf[i] = data[i];
			}
			Frame frame;
			if (frame.read(3, (uint8_t*)send_buf, BUFFSIZE) <= 0) continue;
			if (frame.tcp.dst_port != config->port && frame.udp.dst_port != config->port) continue;
			int wn = frame.write(3, (uint8_t*)send_buf, BUFFSIZE);
			int sn = sendto(sk, send_buf, wn, 0, (sockaddr*)& client_sk_info, sizeof(sockaddr));
		}
	}
}

void Server::recv() {
	int len = sizeof(client_sk_info);
	while (true) {
		int rl = recvfrom(sk, recv_buf, BUFFSIZE, 0, (sockaddr*)& client_sk_info, &len);
		if (rl > 0) {
			Frame frame;
			if (frame.read(3, (uint8_t*)recv_buf, rl) <= 0) continue;
			frame.ipv4.src = config->route.getRoute(frame.ipv4.dst)->addr;

			if (frame.ipv4.protocol == UDPID) {
				frame.udp.src_port = config->port;
			}
			else if (frame.ipv4.protocol == TCPID) {
				frame.tcp.src_port = config->port;
			}
			else {
				continue;
			}
			int wn = frame.write(3, (uint8_t*)recv_buf, BUFFSIZE);

			vector<uint8_t> data;
			for (int i = 0; i < wn; i++) {
				data.push_back(recv_buf[i]);
			}

			tun->write(data, 1);
		}
	}
}