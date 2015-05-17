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

#define SIZE 100

class FileServer {
public:
	FileServer();

    virtual bool receive( unsigned short host_port);

private:
    uint8_t buffer[SIZE];
};

#endif /* SRC_FILE_MANAGER_SERVER_FILESERVER_H_ */
