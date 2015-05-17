#ifndef _FILE_MANAGER_INTERFACE_
#define _FILE_MANAGER_INTERFACE_

#include <string>
#include <cstdint>

class FileManagerInterface {
public:
	virtual bool send(const std::string & host, std::string & file_path, uint64_t from = 0, uint64_t to = -1) = 0;
	virtual ~FileManagerInterface(){};
};

#endif // _FILE_MANAGER_INTERFACE_
