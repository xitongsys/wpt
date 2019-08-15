#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "config.h"
#include "Tun.h"
#include <thread>
#include <iostream>

class Client{
public:
	static const int BUFFSIZE = 2000;

	Config* config;
	Tun* tun;
	SOCKET sk;
	struct sockaddr_in server_sk_info;

	char send_buf[BUFFSIZE];
	char recv_buf[BUFFSIZE];

public:
	Client(Config* config);
	~Client();

	bool start();
	void send();
	void recv();
	void heartbeat();
};

