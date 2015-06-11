#ifndef _APP_H_INCLUDED_
#define _APP_H_INCLUDED_

#include <memory>
#include <utility>
#include <unordered_map>
#include <thread>
#include <map>

#include "../common/p2p_network_interface.h"
#include "../common/file_manager_interface.h"

class App {
public:
    typedef std::unordered_map<std::string, std::vector<uint64_t>> host_id_map;
    typedef struct {
        uint64_t start, size;
        std::vector<std::pair<std::string, uint64_t>> hosts;
    } FileInfo;

    // for each file - vector of FileInfo for each of the chunks
    typedef std::map<std::string, std::vector<FileInfo>> FileStorage;

    typedef struct {
        uint16_t ms_port;
        uint16_t server_port;
        uint16_t file_mgr_port;
        uint16_t app_port;
        std::string main_server;
    } Settings;

    enum class FileAvailability { None, Low, Normal, High };

    App(
        Settings settings,
        std::unique_ptr<FileManagerInterface> fileManager,
        std::unique_ptr<P2PNetworkInterface> networkManager);

    App(const App &) = delete;
    App & operator=(const App &) = delete;
    ~App();

    void run();
    void stop();

    // file managment

    bool importFile(const std::string &);
    bool exportFile(const std::string & storageFile, const std::string & outputFilePath);
    FileAvailability isFileInStorage(const std::string &);


    // map: host -> it's ids
    host_id_map getPeersIds();

    const static uint64_t fileChunkSize = 1 << 20; // 1 mb
private:
    void checkFilesAvailability();

    void listener();


    std::unique_ptr<FileManagerInterface> fileManager;
    std::unique_ptr<P2PNetworkInterface> networkManager;

    std::thread * appThread, * fileMgrThread;
    bool running;
    Settings settings;
    FileStorage storage;
};


#endif // _APP_H_INCLUDED_
