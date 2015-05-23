#include "p2p_main_server.h"
#include "commondefines.h"

#include <cstring>
#include <iostream>
#include <thread>
#include <chrono>

#ifndef C_WIN_SOCK
#include <arpa/inet.h>
#include <errno.h>
#endif

#define MSPORT 23456
#define BUFFER_SIZE 16384

void P2PMainServer::start()
{
    this->socket.makeNonblocking();

	this->socket.bindTo(MSPORT);
	if(!this->socket.listen()){
		std::cerr << "Unsuccessful listening request!\n";
		exit(1);
	}

	char buffer[BUFFER_SIZE];
	memset(buffer, 0, sizeof(buffer));

	std::cout << "Server waiting for connections at port " << MSPORT << std::endl;
	while(true) {
	    std::this_thread::sleep_for(std::chrono::milliseconds(1));
		ClientInfo current_client = this->socket.accept();
		if(current_client.sock_fd == -1) {
            continue;
		}
		handleClientConnect(current_client);
		char* client_ip_addr = inet_ntoa(current_client.addr.sin_addr);
		std::cout << "Accepted connection from " << client_ip_addr << std::endl;
		std::cout << "File descriptor " << current_client.sock_fd << std::endl;
		serveConnectedClients(buffer);
	}
}

size_t P2PMainServer::recv(int fd, void* buf, size_t buf_size, int flags) const
{
    return ::recv(fd, reinterpret_cast<char *>(buf), buf_size, flags);
}

size_t P2PMainServer::send(int fd, const void* buf, size_t msg_size, int flags) const
{
    return ::send(fd, reinterpret_cast<const char *>(buf), msg_size, flags);
}

void P2PMainServer::handleClientConnect(ClientInfo& client)
{
    if(client.sock_fd != INVALID_SOCKFD) {
        client.connected = true;
        client.server_port = 0;
        client.file_mgr_port = 0;
        /** make the socket non-blocking so recv does not block
          * when called in serveConnectedClients
          */

#ifdef C_WIN_SOCK
        u_long mode = 1;
        if (ioctlsocket(client.sock_fd, FIONBIO, &mode) != NO_ERROR) {
#else
        if (fcntl(this->fd, F_SETFL, O_NONBLOCK) == -1) {
#endif
            std::cerr << "Cannot make client socket non-blocking !\n";
            exit(1);
        }
        this->clients.push_back(client);
    }
}

void P2PMainServer::serveConnectedClients(char* in_buffer)
{
    size_t clients_count = this->clients.size();
    Command cmd;
    for(size_t i = 0; i < clients_count; ++i) {
        size_t received_len = recv(clients[i].sock_fd, &cmd, sizeof(cmd));
        if(received_len == sizeof(Command)){
            switch(cmd) {
                case Command::GET_PEERS:
                    sendPeersInfo(i);
                    break;
                case Command::LISTENING_PORT:
                    acceptClientListeningPort(in_buffer, i);
                    break;
                default:
                    break;
            }
        }
    }
}

void P2PMainServer::sendPeersInfo(int out_peer_index) const
{
    std::vector<ServerInfo> connected_peers;

    checkPeers(connected_peers, out_peer_index);

    int count = connected_peers.size();
    char buffer[BUFFER_SIZE];
    memcpy(buffer, &count, sizeof(count));
    int offset = sizeof(count);

    int port_size = sizeof(decltype(ServerInfo::server_port));
    int addr_size = sizeof(decltype(ServerInfo::ip_addr));

    for(int i = 0; i < connected_peers.size(); ++i) {
        memcpy(buffer + offset, &connected_peers[i].ip_addr, addr_size);
        offset += addr_size;
        memcpy(buffer + offset, &connected_peers[i].server_port, port_size);
        offset += port_size;
        memcpy(buffer + offset, &connected_peers[i].file_mgr_port, port_size);
        offset += port_size;
    }
    int msg_size = sizeof(count) + count * (addr_size + 2 * port_size);
    int sent = send(clients[out_peer_index].sock_fd, buffer, msg_size);
    if(sent < msg_size) {
        std::cerr << "Failed sending peers info !" << std::endl;
        exit(1);
    }
    std::cout << "Peer's info sent " << std::endl;
    std::cout << count << " peers connected" << std::endl;
}

void P2PMainServer::checkPeers(std::vector<ServerInfo>& connected_peers, int out_peer_index) const
{
    ServerInfo temp;
    for(int i = 0; i < clients.size(); ++i) {
        if(clients[i].server_port == 0 || i == out_peer_index) {
            continue;
        }
        Socket sock;
        sockaddr_in addr = clients[i].addr;
        addr.sin_port = htons(clients[i].server_port);
        sock.connectTo(&addr);
        /** if connect does not exit it is succesfull */
        temp.ip_addr = addr.sin_addr.s_addr;
        temp.server_port = clients[i].server_port;
        temp.file_mgr_port = clients[i].file_mgr_port;
        connected_peers.push_back(temp);
    }
}

void P2PMainServer::acceptClientListeningPort(char* in_buffer, int client_index)
{
    int offset = 0;
    size_t received = recv(clients[client_index].sock_fd, in_buffer, BUFFER_SIZE);
    if(received <= 0) {
        std::cerr << "No server and file manager ports received !" << std::endl;
        exit(1);
    }
    clients[client_index].server_port = *((uint16_t*)(in_buffer + offset));
    offset += sizeof(clients[client_index].server_port);
    clients[client_index].file_mgr_port = *((uint16_t*)(in_buffer + offset));

    std::cout << "Server port: " << clients[client_index].server_port << std::endl;
    std::cout << "File manager port: " << clients[client_index].file_mgr_port << std::endl;
}
