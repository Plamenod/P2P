#ifndef P2P_MAIN_SERVER_H_
#define P2P_MAIN_SERVER_H_

#include "socket.h"
#include "commondefines.h"

#include <utility>
#include <vector>

#define INITIAL_CLIENTS_COUNT 64

class P2PMainServer{

    struct ClientDescriptor {
        Socket sock;
        uint16_t server_port, file_mgr_port;

        ClientDescriptor(ClientDescriptor && o)
            : sock(std::move(o.sock)), server_port(o.server_port),
            file_mgr_port(o.file_mgr_port) {}

        ClientDescriptor(Socket sock, uint16_t sp, uint16_t fmp)
            : sock(std::move(sock)), server_port(sp), file_mgr_port(fmp) {}

        ClientDescriptor & operator=(ClientDescriptor && o) {
            if (this == &o) {
                return *this;
            }

            sock = std::move(o.sock);
            server_port = o.server_port;
            file_mgr_port = o.file_mgr_port;

            return *this;
        }

        ClientDescriptor(const ClientDescriptor &) = delete;
        ClientDescriptor & operator=(const ClientDescriptor &) = delete;
    };


public:
    P2PMainServer(): isRunning(false) { clients.reserve(INITIAL_CLIENTS_COUNT); };

    P2PMainServer(const P2PMainServer& other) = delete;
    P2PMainServer& operator =(const P2PMainServer& other) = delete;

    ~P2PMainServer() {};

    void start(int port);
    void stop();

private:

    void handleClientConnect(Socket & client);
    void serveConnectedClients(char* in_buffer);
    void acceptClientListeningPort(char* in_buffer, int client_index);
    void sendPeersInfo(int out_peer_index);
    void checkPeers(std::vector<ServerInfo>& connected_peers, int out_peer_index);


    bool isRunning;
    Socket socket;
    uint16_t port;
    std::vector<ClientDescriptor> clients;
};

#endif // P2P_MAIN_SERVER_H_
