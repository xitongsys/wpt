#include "tun.h"
#include <tchar.h>
#include <thread>
#include <iostream>


Tun::Tun(Config* config) {
	handle = NULL;
	this->config = config;
}

Tun::~Tun(){}

vector<uint8_t> Tun::read() {
	vector<uint8_t> res;
	UINT rn = 0, wn = 0;
	WINDIVERT_ADDRESS addr;
	UINT addrLen = sizeof(addr);
	if(!WinDivertRecvEx(handle, read_buf, BUFFSIZE, &rn, 0, &addr, &addrLen, NULL)) {
		return res;
	}

	config->if_index = addr.Network.IfIdx;

	for (int i = 0; i < rn; i++) {
		res.push_back(read_buf[i]);
	}

	return res;
}

bool Tun::write(vector<uint8_t>& data) {
	for (int i = 0; i < data.size(); i++) {
		write_buf[i] = data[i];
	}

	UINT rn = data.size();
	UINT wn = 0;
	WINDIVERT_ADDRESS addr;
	addr.Layer = WINDIVERT_LAYER_NETWORK;
	addr.Impostor = 1;
	addr.Loopback = 0;
	addr.Outbound = 0;
	addr.IPChecksum = 1;
	addr.TCPChecksum = 1;
	addr.UDPChecksum = 1;
	addr.Network.IfIdx = config->if_index;
	addr.Network.SubIfIdx = 0;
	WinDivertSendEx(handle, write_buf, rn, &wn, 0, &addr, sizeof(addr), NULL);
	return true;
}

bool Tun::start() {
	char fmt[] = "(localPort == %d or remotePort == %d) and !loopback and ifIdx == %d and !impostor";
	char filter[1024];
	sprintf_s(filter, 1024, fmt, config->port, config->port, config->if_index);
	handle = WinDivertOpen(filter, WINDIVERT_LAYER_NETWORK, 0, 0);
	return !(handle == INVALID_HANDLE_VALUE);
}

bool Tun::stop() {
	WinDivertClose(handle);
	return true;
}
