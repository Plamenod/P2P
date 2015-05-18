#ifndef FILECLIENT_H_
#define FILECLIENT_H_
#include "socket.h"
#include <utility>
#include <cassert>
#include <vector>
#include <cstdint>

class FileClient {
public:
	FileClient();
	virtual ~FileClient();

    uint64_t send(
        const std::string & host,
        std::string & file_path,
        uint64_t from = 0,
        uint64_t to = -1);

private:
    /**
    * @brief send length of file
    * @param host_socket socket to send length
    * @param length length to send
    * @return true on success, false on failure
    */
    bool sendLength(const Socket& host_socket, uint64_t length);

    /**
    * @brief get file ID from host_socket
    * @param host_socket socket to receive ID from
    * @return 0 on failure
    */
    uint64_t getFileID(const Socket& host_socket);

};

#endif /* FILECLIENT_H_ */
