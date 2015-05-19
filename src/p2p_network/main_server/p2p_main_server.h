#ifndef P2P_MAIN_SERVER_H_
#define P2P_MAIN_SERVER_H_

#include "socket.h"
#include "commondefines.h"

#include <utility>
#include <vector>

#define INITIAL_CLIENTS_COUNT 64

class P2PMainServer{

public:
	P2PMainServer() { clients.reserve(INITIAL_CLIENTS_COUNT); };

	P2PMainServer(const P2PMainServer& other) = delete;
	P2PMainServer& operator =(const P2PMainServer& other) = delete;

    ~P2PMainServer() {};

    void start();

private:
    size_t recv(void* buf, size_t buf_size, int flags = 0) const;
    size_t send(int client_fd, const void* buf, size_t msg_size, int flags = 0) const;
	void handleClientConnect(ClientInfo& client);
	void serveConnectedClients(char* in_buffer);
	void acceptClientListeningPort(char* in_buffer, int client_index);

	Socket socket;
	uint16_t port;
	std::vector<ClientInfo> clients;
};

#endif // P2P_MAIN_SERVER_H_
