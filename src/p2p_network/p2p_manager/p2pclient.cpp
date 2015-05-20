#include "p2pclient.h"
#include <iostream>


#ifndef C_WIN_SOCK
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
#else

#endif

#include <chrono>
#include <thread>
#include <cstring>
#include <sstream>

const int retries = 10;
const uint32_t IN_BUFFERSIZE = 16384; /// default send size
const uint32_t OUT_BUFFERSIZE = 4096;

void P2PClient::setServerIP(const std::string& ip)
{
    server_ip = ip;
}

void P2PClient::connectToServer(const std::string& ip)
{
    setServerIP(ip);
    socket.connectTo(ip, main_server_port);
    sendPortsToMainServer();
}

size_t P2PClient::send(const void* buf, size_t size, int flags) const
{
    return ::send(socket.getFd(), reinterpret_cast<const char *>(buf), size, flags);
}

size_t P2PClient::recv(void* buf, size_t size, int flags) const
{
	return ::recv(socket.getFd(), reinterpret_cast<char *>(buf), size, flags);
}

void P2PClient::sendPortsToMainServer() const
{
    char cmd = Command::LISTENING_PORT;
    char out_buff[OUT_BUFFERSIZE], *buff_ptr = out_buff;
    out_buff[0] = cmd;

    int cmd_size = sizeof(cmd);
    int server_port_size = sizeof(server_port);
    int file_mgr_port_size = sizeof(file_mgr_port);

    memcpy(out_buff + cmd_size, &server_port, server_port_size);
    int offset = cmd_size + server_port_size;
    memcpy(out_buff + offset, &file_mgr_port, file_mgr_port_size);

    int data_size = cmd_size + server_port_size + file_mgr_port_size;
    size_t sent = send(out_buff, data_size);
    if(sent <= 0) {
        std::cerr << "Sending listening port failed" << std::endl;
        exit(1);
    }

    std::cout << "Listening and file manager ports sent !!" << std::endl;
}

void P2PClient::getPeersInfo(std::vector<PeerInfo>& result) const
{
    char cmd = Command::GET_PEERS;
    size_t sent;
    for(int i = 0; i < retries; ++i) {
        sent = send(&cmd, sizeof(cmd));
        if(sent > 0) break;
    }

    if(sent <= 0) {
        std::cerr << "No connection to main server(sending get peers cmd)" << std::endl;
        exit(1);
    }

    receivePeersInfo(result);
}

void P2PClient::receivePeersInfo(std::vector<PeerInfo>& result) const
{
    /** sleep in order to give main server time to answer */
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    size_t received;
    char buffer[IN_BUFFERSIZE], *buff_ptr = buffer;
    received = recv(buffer, IN_BUFFERSIZE);
    if(received <= 0) {
        std::cerr << "No connection to main server(receiving peers info)" << std::endl;
        exit(1);
    }
    int number_of_peers = (int)(*buff_ptr);

    uint8_t ip_addr_part;
    uint32_t ip_addr;
    uint16_t server_port;
    uint16_t file_mgr_port;

    int ip_addr_size = sizeof(ip_addr);
    int numb_of_peers_size = sizeof(number_of_peers);

    PeerInfo peer_info_tmp;

    for(int i = 0; i < number_of_peers; ++i) {
        std::stringstream strstream;
        int offset = numb_of_peers_size + i;
        ip_addr = *((uint32_t*)(buff_ptr + offset));

        for(int j = 0; j < 4; ++j) {
            uint8_t* ip_addr_ptr = (uint8_t*)&ip_addr;
            ip_addr_part = *((uint8_t*)(ip_addr_ptr + j));
            strstream << ip_addr_part;
            if(i < 3) strstream << ".";
        }
        offset += ip_addr_size;
        server_port = *((uint16_t*)(buff_ptr + offset));
        offset += sizeof(server_port);
        file_mgr_port = *((uint16_t*)(buff_ptr + offset));
        strstream << "/" << server_port << "/" << file_mgr_port;

        peer_info_tmp.address = strstream.str();
        peer_info_tmp.connected = true;
        result.push_back(peer_info_tmp);
    }
}
