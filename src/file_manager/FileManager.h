#ifndef FILEMANAGER_H_
#define FILEMANAGER_H_

#include "file_manager_interface.h"
#include "server/FileServer.h"
#include "client/FileClient.h"

class FileManager : public FileManagerInterface {
    FileClient client;
    FileServer server;
public:
    FileManager(uint16_t serverPort);

    uint64_t send(
        const std::string & host,
        std::string & file_path,
        uint64_t from = 0,
        uint64_t to = -1) override;

    std::vector<uint64_t> getMyIds() override;
    std::unique_ptr<char[]> getFile(const std::string & host, uint64_t id) override;

    void run() override;
    void stop() override;

    ~FileManager();
};

#endif // FILEMANAGER_H_