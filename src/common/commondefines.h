#ifndef _COMMONDEFINES_H_
#define _COMMONDEFINES_H_

#include <string>

#if defined(_WIN32)
#define C_WIN_SOCK
#endif

#ifdef C_WIN_SOCK
#include <cstdint>
#include <winsock2.h>
#include <WS2tcpip.h>
#else
#include <unistd.h>
#include <netinet/in.h>
#endif

enum Command: char {
    GET_PEERS = '1',
    LISTENING_PORT = '2',
};

struct PeerInfo {
	std::string address;
	bool connected;
};

struct ServerInfo {
    uint32_t ip_addr;
    uint16_t server_port;
    uint16_t file_mgr_port;
};

struct ClientInfo {
	sockaddr_in addr;
	bool connected;
	int sock_fd;
	uint16_t server_port;
	uint16_t file_mgr_port;
};


#endif

