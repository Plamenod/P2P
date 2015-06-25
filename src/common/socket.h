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

    explicit Socket(int fdesc);
    Socket();

    Socket(Socket&& other); //move constructor
    Socket & operator=(Socket && other); // move assign

    Socket(const Socket& other) = delete;
    Socket& operator =(const Socket& other) = delete;

    ~Socket();

    operator int() const { return fd; };
    int getFd() const { return fd; };

    uint32_t getPeerAddress();
    std::string getPeerName();

    void bindTo(unsigned short port) const;
    int connectTo(const std::string& ip, uint16_t port) const;
    int connectTo(const sockaddr_in* server_addr) const;

    int recv(void * buf, int len, int flags = 0);
    int send(const void * buf, int len, int flags = 0);

    void addOption(int option = SO_REUSEADDR);
    bool makeNonblocking() const;

    bool listen() const;
    ClientInfo accept() const;
    Socket acceptSocket() const;

private:
    sockaddr_in getSockAddr();
    void close();


private:
    int fd;
};

#endif // SOCKET
