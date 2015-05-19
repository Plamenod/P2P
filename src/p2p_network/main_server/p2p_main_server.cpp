#include "p2p_main_server.h"

#include <cstring>
#include <iostream>
#include <thread>
#include <chrono>
#include <arpa/inet.h>

#define MSPORT 3695
#define BUFFER_SIZE 16384

void P2PMainServer::start()
{
	this->socket.bindTo(MSPORT);
	if(!this->socket.listen()){
		std::cerr << "Unsuccessful listening request!\n";
		exit(1);
	}

	this->socket.makeNonblocking();

	char buffer[BUFFER_SIZE];
	memset(buffer, 0, sizeof(buffer));

	std::cout << "Server waiting for connections !" << std::endl;
	while(true) {
	    std::this_thread::sleep_for(std::chrono::milliseconds(1));
		ClientInfo current_client = this->socket.accept();
		handleClientConnect(current_client);
		char* client_ip_addr = inet_ntoa(current_client.addr.sin_addr);
		std::cout << "Accepted connection from " << client_ip_addr << std::endl;
		serveConnectedClients(buffer);
	}
}

size_t P2PMainServer::recv(void* buf, size_t buf_size, int flags) const
{
    return ::recv(socket.getFd(), buf, buf_size, flags);
}

size_t P2PMainServer::send(int client_fd, const void* buf, size_t msg_size, int flags) const
{
    return ::send(client_fd, buf, msg_size, flags);
}

void P2PMainServer::handleClientConnect(ClientInfo& client)
{
    if(client.sock_fd != INVALID_SOCKFD) {
        client.connected = true;
        client.server_port = 0;
        this->clients.push_back(client);
    }
}

void P2PMainServer::serveConnectedClients(char* in_buffer)
{
    size_t clients_count = this->clients.size();
    char cmd;
    for(size_t i = 0; i < clients_count; ++i) {
        size_t received_len = recv(&cmd, sizeof(cmd));
        if(received_len == 1){
            switch(in_buffer[0]) {
                case '1': break;
                case '2':
                    acceptClientListeningPort(in_buffer, i);
                    break;
                default:
                    break;
            }
        }
    }
}

void P2PMainServer::acceptClientListeningPort(char* in_buffer, int client_index)
{
    char cmd = Command::LISTENING_PORT;
    int offset = sizeof(cmd);
    size_t received = recv(in_buffer, BUFFER_SIZE);
    if(received <= 0) {
        std::cerr << "No server and file manager ports received !" << std::endl;
        exit(1);
    }
    clients[client_index].server_port = *((uint16_t*)(in_buffer + offset));
    offset += sizeof(clients[client_index].server_port);
    clients[client_index].file_mgr_port = *((uint16_t*)(in_buffer + offset));
}
