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

P2PClient::P2PClient(uint16_t main_server_port, uint16_t server_port, uint16_t file_mgr_port)
    : main_server_port(main_server_port), server_port(server_port), file_mgr_port(file_mgr_port)
{
}

void P2PClient::setServerIP(const std::string& ip)
{
    server_ip = ip;
}

void P2PClient::setPorts(uint16_t ms_port, uint16_t server_port, uint16_t file_mgr_port)
{
    this->main_server_port = ms_port;
    this->server_port = server_port;
    this->file_mgr_port = file_mgr_port;
}

void P2PClient::connectToServer(const std::string& ip)
{
    std::unique_lock<std::mutex> lock(this->workerMutex);

    setServerIP(ip);
    if (socket.connectTo(ip, main_server_port)) {
        std::cerr << "Cannot connect to server !" << std::endl;
        exit(1);
    }
    std::cout << "Connected to " << ip << " " << main_server_port << std::endl;
    sendPortsToMainServer();

    if (!this->socket.makeNonblocking()) {
        std::cerr << "Failed to make the p2p socket nonblocking " << std::endl;
        exit(1);
    }

    this->isRunning = true;
    this->workerThread = std::thread(&P2PClient::workerLoop, this);
}

void P2PClient::disconnectFromServer()
{
    // wait for all requests to finish
    std::unique_lock<std::mutex> lock(this->workerMutex);

    this->isRunning = false;
    if (this->workerThread.joinable()) {
        this->workerThread.join();
    }

    this->workerThread = std::thread();
    this->socket.close();
}

void P2PClient::workerLoop()
{
    while (this->isRunning) {

        {
            std::unique_lock<std::mutex> lock(workerMutex);
            if (!this->isRunning || this->socket == -1) {
                // woops, someone stopped us
                break;
            }

            Command cmd;
            int recv = this->socket.recv(&cmd, sizeof(cmd));
            if (recv == sizeof(cmd)) {
                if (cmd == PING) {
                    cmd = Command::PONG;
                    if (0 == this->socket.send(&cmd, sizeof(cmd))) {
                        break;
                    }
                } else {
                    // we should not receive anything else than PING from server
                    isRunning = false;
                    break;
                }
            } else if (recv == 0) {
                // remote socket closed connection
                isRunning = false;
                break;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void P2PClient::sendPortsToMainServer()
{
    Command cmd = Command::LISTENING_PORT;
    char out_buff[OUT_BUFFERSIZE], *buff_ptr = out_buff;
    //out_buff[0] = cmd;
    memcpy(out_buff, &cmd, sizeof(cmd));

    int cmd_size = sizeof(cmd);
    int server_port_size = sizeof(server_port);
    int file_mgr_port_size = sizeof(file_mgr_port);

    memcpy(out_buff + cmd_size, &server_port, server_port_size);
    int offset = cmd_size + server_port_size;
    memcpy(out_buff + offset, &file_mgr_port, file_mgr_port_size);

    int data_size = cmd_size + server_port_size + file_mgr_port_size;
    int sent = this->socket.send(out_buff, data_size);
    if (sent <= 0) {
        std::cerr << "Sending listening port failed" << std::endl;
        exit(1);
    }

    std::cout << "Listening and file manager ports sent !!" << std::endl;
    std::cout << sent << " bytes" << std::endl;
    std::cout << "Server port:" << server_port << "\nFile mgr port: " << file_mgr_port << std::endl;
}

void P2PClient::getPeersInfo(std::vector<PeerInfo>& result)
{
    Command cmd = Command::GET_PEERS;
    int sent = -1;

    {
        std::unique_lock<std::mutex> lock(this->workerMutex);
        sent = this->socket.send(&cmd, sizeof(cmd));
        if (sent == sizeof(cmd)) {
            // we need the lock to receive the data or we might receieve it as a command
            // in the workerLoop
            return receivePeersInfo(result);
        }
    }

    if (sent == 0) {
        this->disconnectFromServer();
    } else if (sent == -1 && this->socket.wouldHaveBlocked()) {
        std::cout << "Would have blocked on send - return no peers";
    }
}

void P2PClient::receivePeersInfo(std::vector<PeerInfo>& result)
{
    /** sleep in order to give main server time to answer */
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    int received;
    char buffer[IN_BUFFERSIZE], *buff_ptr = buffer;
    received = this->socket.recv(buffer, IN_BUFFERSIZE);
    if (received <= 0) {
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

    int sizeof_peer = sizeof(ip_addr) + sizeof(server_port) + sizeof(file_mgr_port);
    for (int i = 0; i < number_of_peers; ++i) {
        std::stringstream strstream;

        int offset = numb_of_peers_size + i * sizeof_peer;
        ip_addr = *((uint32_t*)(buff_ptr + offset));

        for (int j = 0; j < 4; ++j) {
            uint8_t* ip_addr_ptr = (uint8_t*)&ip_addr;
            ip_addr_part = *((uint8_t*)(ip_addr_ptr + j));
            strstream << (int)ip_addr_part;
            if (j < 3) strstream << ".";
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
