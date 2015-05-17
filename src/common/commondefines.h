#ifndef _COMMONDEFINES_H_
#define _COMMONDEFINES_H_

#include <string>
#include <netinet/in.h>

enum Command: char {
    GET_PEERS = '1',
    LISTENING_PORT = '2',
};

struct PeerInfo {
	std::string address;
	bool connected;
};

struct ClientInfo{
	sockaddr_in addr;
	bool connected;
	int sock_fd;
	uint16_t server_port;
};


#endif

