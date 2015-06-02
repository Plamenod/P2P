#include "FileManager.h"

FileManager::FileManager(): server(), client() {
}

uint64_t FileManager::send(
    const std::string & host,
    std::string & file_path,
    uint64_t from,
    uint64_t to) {

    return client.send(host, file_path, from, to);
}

std::vector<uint64_t> FileManager::getMyIds() {
    return std::vector<uint64_t>();
    //return server.getMyIds();
}

std::unique_ptr<char[]> FileManager::getFile(const std::string & host, uint64_t id) {
    return std::move(client.getFile(host, id));
}

void FileManager::run() {
    //server.run();
}

FileManager::~FileManager() {

}
