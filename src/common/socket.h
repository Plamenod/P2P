#ifndef SOCKET_H_
#define SOCKET_H_


#include "commondefines.h"

#include <string>

#ifdef C_WIN_SOCK
#include <winsock2.h>
#include <WS2tcpip.h>
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#endif

#define PROTOCOL 0
#define BACKLOG 10
#define INVALID_SOCKFD -1


class Socket{

public:

	Socket(int fdesc = INVALID_SOCKFD);

	Socket(const Socket& other) = delete;
	Socket& operator =(const Socket& other) = delete;

	~Socket();

	operator int() const { return fd; };
	int getFd() const { return fd; };

	void bindTo(unsigned short port) const;
	int connectTo(const std::string& ip, uint16_t port) const;
	int connectTo(const sockaddr_in* server_addr) const;

	void addOption(int option = SO_REUSEADDR);
	void makeNonblocking() const;

	bool listen() const;
	ClientInfo accept() const;

private:
	int fd;
};

#endif // SOCKET
