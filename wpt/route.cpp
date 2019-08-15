#include "route.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <cstdio>
#include <iostream>


#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

RouteItem::RouteItem() {
	addr = 0;  dst = 0; mask = 0; next = 0; ifIndex = 0;
}

RouteItem::RouteItem(uint32_t addr, uint32_t dst, uint32_t mask, uint32_t next, int ifIndex){
	this->addr = addr;
	this->dst = dst;
	this->mask = mask;
	this->next = next;
	this->ifIndex = ifIndex;
}

Route::Route() {
	PMIB_IPFORWARDTABLE pTable;
	DWORD dwSize = 0, dwRetVal = 0;
	pTable = (MIB_IPFORWARDTABLE*)malloc(sizeof(MIB_IPFORWARDTABLE));
	if (pTable == NULL) {
		std::cout << "error allocating memory" << std::endl;
		exit(-1);
	}

	if (GetIpForwardTable(pTable, &dwSize, 0) == ERROR_INSUFFICIENT_BUFFER) {
		free(pTable);
		pTable = (MIB_IPFORWARDTABLE*)malloc(dwSize);
		if (pTable == NULL) {
			std::cout << "error allocating memory" << std::endl;
			exit(-1);
		}
	}

	if ((dwRetVal = GetIpForwardTable(pTable, &dwSize, 0)) == NO_ERROR) {
		int n = pTable->dwNumEntries;
		for (int i = 0; i < n; i++) {
			RouteItem it;
			it.dst = revIp((uint32_t)pTable->table[i].dwForwardDest);
			it.mask = revIp((uint32_t)pTable->table[i].dwForwardMask);
			it.next = revIp((uint32_t)pTable->table[i].dwForwardNextHop);
			it.ifIndex = pTable->table[i].dwForwardIfIndex;
			routes.push_back(it);
		}
		free(pTable);
	}
	else {
		free(pTable);
		std::cout << "get route table error" << std::endl;
		exit(-1);
	}

	PIP_ADAPTER_INFO p_adapter_info;
	PIP_ADAPTER_INFO p_adapter;

	ULONG len = sizeof(IP_ADAPTER_INFO);
	p_adapter_info = (IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));
	if (p_adapter_info == NULL) {
		std::cout << "can't get adapter info" << std::endl;
		exit(-1);
	}

	if (GetAdaptersInfo(p_adapter_info, &len) == ERROR_BUFFER_OVERFLOW) {
		free(p_adapter_info);
		p_adapter_info = (IP_ADAPTER_INFO*)malloc(len);
		if (p_adapter_info == NULL) {
			std::cout << "can't get adapter info" << std::endl;
			exit(-1);
		}
	}

	if (GetAdaptersInfo(p_adapter_info, &len) == NO_ERROR) {
		p_adapter = p_adapter_info;
		while (p_adapter) {
			int index = p_adapter->Index;
			uint32_t ip = str2ip(p_adapter->IpAddressList.IpAddress.String);
			for (int i = 0; i < routes.size(); i++) {
				if (routes[i].ifIndex == index) {
					routes[i].addr = ip;
				}
			}
			p_adapter = p_adapter->Next;
		}
		free(p_adapter_info);
	}
	else {
		free(p_adapter_info);
		std::cout << "can't get adapter info" << std::endl;
		exit(-1);
	}

}

RouteItem* Route::getRoute(uint32_t dst) {
	int ln = routes.size();
	for (int i = ln - 1; i >= 0; i--) {
		if ((dst & routes[i].mask) == routes[i].dst) {
			return &routes[i];
		}
	}
	return NULL;
}

string Route::toString() {
	char fmt[] = "addr: %s, dst: %s, mask: %s, next: %s, index: %d\n";
	char buf[1024];
	string res = "";
	for (int i = 0; i < routes.size(); i++) {
		sprintf_s(buf, 1024, fmt, 
			ip2str(routes[i].addr).c_str(),
			ip2str(routes[i].dst).c_str(), 
			ip2str(routes[i].mask).c_str(), 
			ip2str(routes[i].next).c_str(), 
			routes[i].ifIndex);
		res += buf;
	}
	return res;
}