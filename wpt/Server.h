#pragma once
#include <WinSock2.h>
#include "config.h"
#include "Tun.h"
#include <thread>
#include <iostream>

class Server{
public:
	static const int BUFFSIZE = 2000;

	Config* config;
	Tun* tun;
	SOCKET sk;
	struct sockaddr_in server_sk_info, client_sk_info;

	char send_buf[BUFFSIZE];
	char recv_buf[BUFFSIZE];

public:
	Server(Config* config);
	~Server();

	bool start();
	void send();
	void recv();
};

