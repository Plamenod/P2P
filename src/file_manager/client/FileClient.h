#ifndef FILECLIENT_H_
#define FILECLIENT_H_
#include "socket.h"
#include <utility>
#include <cassert>
#include <vector>
#include <cstdint>
#include <memory>
#include "Encryption.h"
#define MAX_KEY_LENGTH 5

class FileClient {
public:
    FileClient();
    virtual ~FileClient();

    /**
    * @brief send file with range (from, to) to host
    * @return file ID on success, 0 on failure
    */
    uint64_t send(
        const std::string & host,
        const std::string & file_path,
        uint64_t from = 0,
        uint64_t to = -1);

    /**
    * @brief get content of file with id from host
    * @return pointer to file content on success, nullptr on failure
    */
    std::unique_ptr<char[]> getFile(const std::string& host, uint64_t id);
private:
    /**
    * @brief send number to host_socket
    * @param host_socket socket to send length
    * @param number number to send
    * @return true on success, false on failure
    */
    bool sendNumber(Socket& host_socket, uint64_t number);

    /**
    * @brief get file ID from host_socket
    * @param host_socket socket to receive ID from
    * @return 0 on failure
    */
    uint64_t getFileID(Socket& host_socket);

    /**
     * @return key that maps to id on success, empty string on failure
     */
	Encryption getKeyFromId(uint64_t id);

    bool writeKeyToFile(const Encryption::KeySave & );
};

#endif /* FILECLIENT_H_ */
