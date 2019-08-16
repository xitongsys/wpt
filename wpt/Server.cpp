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
		//std::cout << "tun read" << data.size() << std::endl;
		if (data.size() > 0 && data.size() < BUFFSIZE) {
			for (int i = 0; i < data.size(); i++) {
				send_buf[i] = data[i];
			}
			Frame frame;
			if (frame.read(3, (uint8_t*)send_buf, BUFFSIZE) <= 0) continue;
			int wn = frame.write(3, (uint8_t*)send_buf, BUFFSIZE);
			int sn = sendto(sk, send_buf, wn, 0, (sockaddr*)& client_sk_info, sizeof(sockaddr));

			//std::cout << "send to client: " << sn << std::endl;
		}
	}
}

void Server::recv() {
	int len = sizeof(client_sk_info);
	while (true) {
		int rl = recvfrom(sk, recv_buf, BUFFSIZE, 0, (sockaddr*)& client_sk_info, &len);
		//std::cout << "=====" << rl << std::endl;
		if (rl > 0) {
			Frame frame;
			//std::cout << "recv data: " << rl << std::endl;
			if (frame.read(3, (uint8_t*)recv_buf, rl) <= 0) continue;

			//std::cout << "recv from client: " << rl << std::endl;
			//std::cout << frame.ipv4.to_string() << frame.udp.to_string() << std::endl;

			if (config->direction == 0) {//client->server
				frame.ipv4.src = config->route.getRoute(frame.ipv4.dst)->addr;

				if (frame.ipv4.protocol == UDPID) {
					if (config->client2server.find(frame.udp.src_port) == config->client2server.end()) continue;
					frame.udp.src_port = config->client2server[frame.udp.src_port];
				}
				else if (frame.ipv4.protocol == TCPID) {
					if (config->client2server.find(frame.tcp.src_port) == config->client2server.end()) continue;
					frame.tcp.src_port = config->client2server[frame.tcp.src_port];
				}
				else {
					continue;
				}
			}
			else {//server->client
				frame.ipv4.dst = config->route.getRoute(client_sk_info.sin_addr.s_addr)->addr;
				
				if (frame.ipv4.protocol == UDPID) {
					if (config->client2server.find(frame.udp.dst_port) == config->client2server.end()) continue;
					frame.udp.dst_port = config->client2server[frame.udp.dst_port];
				}
				else if (frame.ipv4.protocol == TCPID) {
					if (config->client2server.find(frame.tcp.dst_port) == config->client2server.end()) continue;
					frame.tcp.dst_port = config->client2server[frame.tcp.dst_port];
				}
				else {
					continue;
				}
			}

			//std::cout << "nat: " << rl << std::endl;
			//std::cout << frame.ipv4.to_string() << frame.udp.to_string() << std::endl;
			
			int wn = frame.write(3, (uint8_t*)recv_buf, BUFFSIZE);
			vector<uint8_t> data;
			for (int i = 0; i < wn; i++) {
				data.push_back(recv_buf[i]);
			}
			if (config->direction == 0) {//client->server
				tun->write(data, 1);
			}
			else {//server->client
				tun->write(data, 0);
			}
		}
	}
}