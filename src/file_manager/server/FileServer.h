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

private:

    void send_info_file_to_client(int);
    void send_file_to_client(int);
    /*append current buffer*/
    int append_to_file(int);
    /*append only size of file and unique id*/
    int initial_append(int);
    uint64_t event_type(int);

    std::unique_ptr<char[]> buffer;
    Socket socket;
    FILE* fd;
    InfoData info;

    int listen(int);
    bool recieve_size_of_file(int);
    uint64_t get_id_by_client(int);

    bool isBind;
    
    int  connection_fd;

//    std::vector<uint64_t> all_ids;
};

#endif /* SRC_FILE_MANAGER_SERVER_FILESERVER_H_ */
