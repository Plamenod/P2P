#ifndef FILECLIENT_H_
#define FILECLIENT_H_
#include "file_manager_interface.h"

class FileClient : public FileManagerInterface{
public:
	FileClient();
	virtual ~FileClient();

    bool send(
        const std::string & host,
        const unsigned short host_port,
        std::string & file_path,
        uint64_t from = 0,
        uint64_t to = -1) override;
};

#endif /* FILECLIENT_H_ */
