#include "socket.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
//#include <netinet/in.h>

#ifdef C_WIN_SOCK
	#include <winsock2.h>
	#include <WS2tcpip.h>
#else
	#define SOCKET_ERROR -1
	#include <unistd.h>
	#include <arpa/inet.h>
	#include <sys/fcntl.h>
#endif




Socket::Socket(int fdesc) : fd(fdesc)
{
#ifdef C_WIN_SOCK
	// init MS lib
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

	this->fd = socket(AF_INET, SOCK_STREAM, PROTOCOL);

	if(this->fd < 0){
		std::cerr << "Cannot create socket !\n";
		exit(1);
	}
}


Socket::~Socket()
{
#ifdef C_WIN_SOCK
	closesocket(this->fd);
#else
	close(this->fd);
#endif
}


void Socket::bindTo(unsigned short port) const
{
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if(addr.sin_addr.s_addr == -1){
		std::cerr << "Socket::bind_to : error converting address to int!\n";
		exit(1);
	}


	int yes = 1;
	if (setsockopt(this->fd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&yes), sizeof(int)) == -1) {

#ifdef C_WIN_SOCK
		closesocket(this->fd);
#else
		close(this->fd);
#endif
		std::cerr << "Failed to setsockop SO_REUSEADDR\n";
		exit(1);
	}


	if(bind(this->fd, (sockaddr *) &addr, sizeof(addr)) != 0){
		std::cerr << "Unsuccessful binding!\n";
		exit(1);
	}
}


int Socket::connectTo(const std::string& ip, uint16_t port) const
{
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

	if(server_addr.sin_addr.s_addr == (-1)) {
		return -1;
	}

	return connect(this->fd, (sockaddr*)&server_addr, sizeof(server_addr));
}

int Socket::connectTo(const sockaddr_in* server_addr) const
{
    return connect(this->fd, (sockaddr*)server_addr, sizeof(*server_addr));
}

void Socket::addOption(int option)
{
	int opt_val;
	setsockopt(this->fd, SOL_SOCKET, option, reinterpret_cast<const char*>(&opt_val), sizeof(int));
}


void Socket::makeNonblocking() const
{

#ifdef C_WIN_SOCK
	u_long mode = 1;
	if (ioctlsocket(this->fd, FIONBIO, &mode) != NO_ERROR) {
#else
	if(fcntl(this->fd, F_SETFL, O_NONBLOCK) == -1) {
#endif
		std::cerr << "Cannot make socket nonblocking!\n";
		exit(1);
	}
}


bool Socket::listen() const
{
	return (::listen(this->fd, BACKLOG) == 0);
}


ClientInfo Socket::accept() const
{
	ClientInfo info;
	socklen_t addr_size = sizeof(info.addr);
	info.sock_fd = ::accept(this->fd, (sockaddr *) &info.addr, &addr_size);

	/*if(info.sock_fd != INVALID_SOCKFD || true){
		std::cerr << "Error accepting connection request!\n";
		exit(1);
	}*/

	return info;
}
