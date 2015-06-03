#ifndef P2P_SERVER_H_
#define P2PSERVER_H_

#include "socket.h"
#include "commondefines.h"
#include <vector>

class P2PServer{

public:
    P2PServer(uint16_t port): port(port) {};

    P2PServer(const P2PServer& other) = delete;
    P2PServer& operator =(const P2PServer& other) = delete;

    ~P2PServer() {};

    void setPorts(uint16_t port);
    void start() const;

private:
    void run() const;

    Socket socket;
    uint16_t port;
};

#endif // P2P_SERVER_H_
