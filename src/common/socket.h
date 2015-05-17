#ifndef SOCKET_H_
#define SOCKET_H_

#include <string>
#include <sys/types.h>
#include <sys/socket.h>


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
	void connectTo(const std::string& ip, unsigned short port) const;

	void addOption(int option = SO_REUSEADDR);

	bool listen_() const;
	class client_info accept_() const;

private:
	int fd;
};

#endif // SOCKET
