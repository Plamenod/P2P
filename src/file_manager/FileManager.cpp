#include "FileManager.h"

using namespace std;

FileManager::FileManager(uint16_t serverPort, std::string dbFilePath): server(serverPort, move(dbFilePath)), client() {
}

uint64_t FileManager::send(
    const std::string & host,
    const std::string & file_path,
    uint64_t from,
    uint64_t to) {

    return client.send(host, file_path, from, to);
}

std::vector<uint64_t> FileManager::getMyIds() {
    return server.getAllIds();
}

std::unique_ptr<char[]> FileManager::getFile(const std::string & host, uint64_t id) {
    return std::move(client.getFile(host, id));
}

void FileManager::stop() {
    server.stop();
}

void FileManager::run() {
    server.run();
}

FileManager::~FileManager() {

}
