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
	FileServer(int port);
	~FileServer();

    virtual bool receive();
    // to get current max id and set next free id
    std::vector<uint64_t> get_all_ids();
    void run();
    void stop();

protected:
    bool doesFileExist();
    int getPort();
    void setPort(int new_port);
    void recover_server();
    void set_next_free_id();
    uint64_t seek_2_file(uint64_t id);

private:

    void send_info_file_to_client(int);
    void send_file_to_client(int);
    /*append current buffer*/
    int append_to_file(int);
    /*append only size of file and unique id*/
    uint64_t initial_append(int);
    uint64_t event_type(int);

    void open_file(const char* moode);
    void close_file();

    std::unique_ptr<char[]> buffer;
    Socket socket;
    FILE* fd;
    InfoData info;

    int listen(int);
    bool recieve_size_of_file(int);
    uint64_t get_id_by_client(int);

    bool isBind;

    int port;
    int  connection_fd;
    uint64_t next_free_id;
    std::vector<uint64_t> all_ids;

    uint64_t file_size;
    bool isRun;
};

#endif /* SRC_FILE_MANAGER_SERVER_FILESERVER_H_ */
