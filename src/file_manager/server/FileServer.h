/*
 * FileServer.h
 *
 *  Created on: May 16, 2015
 *      Author: plamen
 */

#ifndef SRC_FILE_MANAGER_SERVER_FILESERVER_H_
#define SRC_FILE_MANAGER_SERVER_FILESERVER_H_

#include <string>
#include <cstdint>
#include "file_manager_interface.h"

#define SIZE_BUFFER 4000
#include <memory>

class FileServer {
public:
	FileServer();

    virtual bool receive( unsigned short host_port);

private:
    std::unique_ptr<char[]> buffer;
};

#endif /* SRC_FILE_MANAGER_SERVER_FILESERVER_H_ */
