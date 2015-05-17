#ifndef P2P_MAIN_SERVER_H_
#define P2P_MAIN_SERVER_H_

#include "socket.h"
#include "commondefines.h"

#include <vector>

#define MSPORT 3695
#define INITIAL_CLIENTS_COUNT 64

class P2PMainServer{

public:
	P2PMainServer() { clients.reserve(INITIAL_CLIENTS_COUNT); };

	P2PMainServer(const P2PMainServer& other) = delete;
	P2PMainServer& operator =(const P2PMainServer& other) = delete;

    ~P2PMainServer() {};

    void start();

private:
	Socket socket;
	short port;
	std::vector<ClientInfo> clients;

};

#endif // P2P_MAIN_SERVER_H_
