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
	bool connected;
	int sock_fd;
};


#endif

