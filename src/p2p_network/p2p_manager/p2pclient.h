#ifndef P2PCLIENT_H_INCLUDED
#define P2PCLIENT_H_INCLUDED

#include "socket.h"
#include "commondefines.h"
#include <string>
#include <vector>
#include <thread>
#include <mutex>

class P2PClient
{
public:
    P2PClient(uint16_t main_server_port, uint16_t server_port, uint16_t file_mgr_port);

    P2PClient(const P2PClient&) = delete;
    P2PClient& operator=(const P2PClient&) = delete;

    void setServerIP(const std::string& ip);
    void setPorts(uint16_t ms_port, uint16_t server_port, uint16_t file_mgr_port);

    void connectToServer(const std::string& ip);
    void disconnectFromServer();

    void getPeersInfo(std::vector<PeerInfo>& result);

private:
    void workerLoop();

    void receivePeersInfo(std::vector<PeerInfo>& result);
    void sendPortsToMainServer();

private:

    Socket socket;

    // used to protect getPeerInfo
    std::mutex workerMutex;
    std::thread workerThread;
    bool isRunning;

    uint16_t main_server_port;
    uint16_t server_port;
    uint16_t file_mgr_port;
    std::string server_ip;
};

#endif // P2PCLIENT_H_INCLUDED
