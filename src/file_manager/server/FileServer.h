#ifndef SRC_FILE_MANAGER_SERVER_FILESERVER_H_
#define SRC_FILE_MANAGER_SERVER_FILESERVER_H_

#include <string>
#include <cstdint>
#include "file_manager_interface.h"

#define SIZE_BUFFER 4000
#include <memory>
#include "socket.h"
#include <cstdint>

struct InfoData{
    uint64_t size_of_file;
    uint64_t id;
};

class FileServer {
public:
	FileServer();
	~FileServer();

    virtual bool receive( unsigned short host_port);

    /*append only size of file and unique id*/
    virtual bool initial_append(int);

    /*append current buffer*/
    virtual bool append_to_file(int);

private:
    std::unique_ptr<char[]> buffer;
    Socket socket;
    FILE* fd;
    InfoData info;

//    std::vector<uint64_t> all_ids;
};

#endif /* SRC_FILE_MANAGER_SERVER_FILESERVER_H_ */
