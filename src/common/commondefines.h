#ifndef _COMMONDEFINES_H_
#define _COMMONDEFINES_H_

#include <string>
#include <netinet/in.h>

enum Command: char {
    GET_PEERS = '1',
};

struct PeerInfo {
	std::string address;
	bool connected;
};


struct ClientInfo{
	sockaddr_in addr;
	int sock_fd;
};

struct IpInfo {
    unsigned int ip_address;
    unsigned short port;
};

#endif

