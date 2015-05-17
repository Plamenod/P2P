#ifndef FILECLIENT_H_
#define FILECLIENT_H_
#include "file_manager_interface.h"
#include "socket.h"

class FileClient : public FileManagerInterface{
public:
	FileClient();
	virtual ~FileClient();

    uint64_t send(
        const std::string & host,
        const unsigned short host_port,
        std::string & file_path,
        uint64_t from = 0,
        uint64_t to = -1) override;

private:
    /**
    * @brief send length of file
    * @param host_socket socket to send length
    * @param length length to send
    */
    bool sendLength(const Socket& host_socket, uint64_t length);

    /**
    * @brief get file ID from host_socket
    * @param host_socket socket to receive ID from
    */
    uint64_t getFileID(const Socket& host_socket);
};

#endif /* FILECLIENT_H_ */
