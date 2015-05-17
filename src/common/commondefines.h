#ifndef _COMMONDEFINES_H_
#define _COMMONDEFINES_H_

#include <string>
#include <netinet/in.h>

struct PeerInfo {
	std::string address;
	bool connected;
};


struct ClientInfo{
	sockaddr_in addr;
	int sock_fd;
};

#endif

