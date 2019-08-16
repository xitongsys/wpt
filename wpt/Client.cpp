#include "Client.h"

Client::Client(Config* config) {
	this->config = config;
	this->tun = new Tun(config);
}

Client::~Client() {}

bool Client::start() {
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
	inet_pton(AF_INET, config->server_ip.c_str(), (void*)& server_sk_info.sin_addr.S_un.S_addr);

	sk = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sk == SOCKET_ERROR) {
		return false;
	}

	u_long mode = 1;
	ioctlsocket(sk, FIONBIO, &mode);

	std::thread send_thread(&Client::send, this);
	std::thread recv_thread(&Client::recv, this);
	std::thread heartbeat_thread(&Client::heartbeat, this);

	heartbeat_thread.join();
	send_thread.join();
	recv_thread.join();
}

void Client::heartbeat() {
	while (true) {
		sendto(sk, send_buf, 2, 0, (sockaddr*)& server_sk_info, sizeof(sockaddr));
		Sleep(1000 * 10);
	}
}

void Client::send() {
	while (true) {
		vector<uint8_t> data = tun->read();
		if (data.size() > 0 && data.size() < BUFFSIZE) {
			for (int i = 0; i < data.size(); i++) {
				send_buf[i] = data[i];
			}

			Frame frame;
			if (frame.read(3, (uint8_t*)send_buf, BUFFSIZE) <= 0) continue;
			int wn = frame.write(3, (uint8_t*)send_buf, BUFFSIZE);
			sendto(sk, send_buf, wn, 0, (sockaddr*)& server_sk_info, sizeof(sockaddr));

			std::cout << "tun read: " << frame.ipv4.to_string() << frame.udp.to_string() << std::endl;
		}
	}
}

void Client::recv() {
	int len = sizeof(server_sk_info);
	while (true) {
		int rl = recvfrom(sk, recv_buf, BUFFSIZE, 0, (sockaddr*)& server_sk_info, &len);
		if (rl > 0) {
			Frame frame;
			if (frame.read(3, (uint8_t*)recv_buf, rl) <= 0) continue;

			std::cout << "recv " << frame.ipv4.to_string() << frame.udp.to_string() << std::endl;

			if (config->direction == 0) {//client->server
				frame.ipv4.dst = config->gateway->addr;

				if (frame.ipv4.protocol == UDPID) {
					if (config->server2client.find(frame.udp.dst_port) == config->server2client.end()) continue;
					frame.udp.dst_port = config->server2client[frame.udp.dst_port];
				}
				else if (frame.ipv4.protocol == TCPID) {
					if (config->server2client.find(frame.tcp.dst_port) == config->server2client.end()) continue;
					frame.tcp.dst_port = config->server2client[frame.tcp.dst_port];
				}
				else {
					continue;
				}
			}
			else {//server->client
				frame.ipv4.src = config->route.getRoute(frame.ipv4.dst)->addr;

				if (frame.ipv4.protocol == UDPID) {
					if (config->server2client.find(frame.udp.src_port) == config->server2client.end()) continue;
					frame.udp.src_port = config->server2client[frame.udp.src_port];
				}
				else if (frame.ipv4.protocol == TCPID) {
					if (config->server2client.find(frame.tcp.src_port) == config->server2client.end()) continue;
					frame.tcp.src_port = config->server2client[frame.tcp.src_port];
				}
				else {
					continue;
				}
			}

			std::cout << "nat " << frame.ipv4.to_string() << frame.udp.to_string() << std::endl;

			int wn = frame.write(3, (uint8_t*)recv_buf, BUFFSIZE);
			vector<uint8_t> data;
			for (int i = 0; i < wn; i++) {
				data.push_back(recv_buf[i]);
			}

			if (config->direction == 0) {//client->server
				tun->write(data, 0);
			}
			else {
				tun->write(data, 1);
			}
		}
	}
}