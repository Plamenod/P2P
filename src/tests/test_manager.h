#ifndef _TEST_MANAGER_H_INCLUDED_
#define _TEST_MANAGER_H_INCLUDED_

#include "app.h"
#include "p2p_network_interface.h"
#include "p2p_main_server.h"

#include <thread>
#include <mutex>
#include <utility>

typedef std::pair<bool, std::string> TestError;

std::unique_ptr<App> createApp(int fileMgrPort, int p2pPort, int msPort, std::string savePath);

TestError filesEqual(const std::string & left, const std::string & right);
TestError generateFile(const std::string & path, uint64_t size, const std::string & data = "abcdefghijklmnopqrstuvwxyz");
TestError removeFile(const std::string & path);

class InstanceManager {
public:
	/**
	* @brief Shared pointers created from InstanceManager will clean all files left
	* by the App class
	*/
	typedef std::shared_ptr<App> AppPtr;

	AppPtr getNewAppInstance();

    std::string getNewTmpFilename() const;

	void startMs();
	void stopMs();

	static InstanceManager & getInstance();

	void clear();

	InstanceManager(const InstanceManager &) = delete;
	InstanceManager & operator=(InstanceManager &) = delete;

	~InstanceManager();
private:

    bool fileExists(const std::string & fName) const;

	InstanceManager() {}

private:
	static const int msPort;
	static int startPort;
	static int startSavePath;

	std::mutex mx;
	std::vector<std::shared_ptr<App>> apps;
	std::unique_ptr<P2PMainServer> ms;
	std::thread worker;
	bool isRunning;
};


class TestCaseBase {
public:

	// on Error in setUp, tearDown and run should not be called
	virtual TestError setUp() { return TestError{true, ""}; }
	virtual TestError tearDown() { return TestError{true, ""}; }
	virtual TestError run() = 0;

	virtual ~TestCaseBase() {
        for (auto & cb : atDtor) {
            cb();
        }
    }
protected:
    std::vector<std::function<void()>> atDtor;
};


#endif // _TEST_MANAGER_H_INCLUDED_
