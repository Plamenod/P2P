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
    uint64_t sizeOfFile;
    uint64_t id;
};

class FileServer {
public:
    FileServer(int port);
    ~FileServer();

    virtual bool receive();
    // to get current max id and set next free id
    std::vector<uint64_t> getAllIds();
    void run();
    void stop();

protected:
    bool doesFileExist();
    int getPort();
    void setPort(int new_port);
    void recoverServer();
    void set_nextFreeId();
    uint64_t seek2File(uint64_t id);

private:

    void sendInfoFileToClient(int);
    void sendFileToClient(int);
    /*append current buffer*/
    int appendToFile(int);
    /*append only size of file and unique id*/
    uint64_t initialAppend(int);
    uint64_t eventType(int);

    void open_file(const char* moode);
    void close_file();

    std::unique_ptr<char[]> buffer;
   // Socket socket;
    FILE* fd;
    InfoData info;

    int listen(int, Socket&);
    bool recieveSizeOfFile(int);
    uint64_t getIdByClient(int);

    bool isBind;

    int port;
    int  connection;
    uint64_t nextFreeId;
    std::vector<uint64_t> all_ids;

    uint64_t fileSize;
    bool isRun;

    void openFile();
    void closeFile();
};

#endif /* SRC_FILE_MANAGER_SERVER_FILESERVER_H_ */
