#ifndef FILECLIENT_H_
#define FILECLIENT_H_
#include "file_manager_interface.h"
#include "socket.h"
#include <utility>
#include <cassert>

class FileClient : public FileManagerInterface{
public:
	FileClient();
	virtual ~FileClient();

	std::vector<uint64_t> getIds() override {
		assert(false);
		return std::vector<uint64_t>();
	};

    uint64_t send(
        const std::string & host,
        std::string & file_path,
        uint64_t from = 0,
        uint64_t to = -1) override;

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


    std::string getHost(const std::string& host);

    unsigned short getPort(const std::string& host);

    std::vector<std::string> split(std::string str, const char* delimiter);
};

#endif /* FILECLIENT_H_ */
