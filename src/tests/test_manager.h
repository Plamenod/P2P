#ifndef _TEST_MANAGER_H_INCLUDED_
#define _TEST_MANAGER_H_INCLUDED_

#include "app.h"
#include "p2p_network_interface.h"
#include "p2p_main_server.h"

#include <thread>
#include <mutex>
#include <utility>

std::shared_ptr<App> createApp(int fileMgrPort, int p2pPort, int msPort, std::string savePath);
std::pair<bool, std::string> filesEqual(const std::string & left, const std::string & right);

class InstanceManager {
public:
	std::shared_ptr<App> nextNonColidingApp();

	void startMs();
	void stopMs();

	static InstanceManager & getInstance();

	void clear();

	InstanceManager(const InstanceManager &) = delete;
	InstanceManager & operator=(InstanceManager &) = delete;


private:
	InstanceManager() {}

private:
	static const int msPort = 2000;
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

	virtual void setUp() {}
	virtual void tearDown() {}
	virtual bool run() = 0;
};


#endif // _TEST_MANAGER_H_INCLUDED_
