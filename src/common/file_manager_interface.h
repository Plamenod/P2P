#ifndef _FILE_MANAGER_INTERFACE_
#define _FILE_MANAGER_INTERFACE_

#include <string>
#include <cstdint>
#include <vector>
#include <memory>

class FileManagerInterface {
public:

	virtual uint64_t send(
		const std::string & host,
		std::string & file_path,
		uint64_t from = 0,
		uint64_t to = -1) = 0;

	virtual std::vector<uint64_t> getMyIds() = 0;
    virtual std::unique_ptr<char[]> getFile(const std::string & host, uint64_t id) = 0;

    virtual void run() = 0;

	virtual ~FileManagerInterface(){};
};

#endif // _FILE_MANAGER_INTERFACE_
