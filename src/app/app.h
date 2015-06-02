#ifndef _APP_H_INCLUDED_
#define _APP_H_INCLUDED_

#include <memory>
#include <utility>
#include <unordered_map>
#include <thread>

#include "../common/p2p_network_interface.h"
#include "../common/file_manager_interface.h"

class App {
	std::unique_ptr<FileManagerInterface> fileManager;
	std::unique_ptr<P2PNetworkInterface> networkManager;

	std::thread * appThread;
	bool running;
public:
	App(
		std::unique_ptr<FileManagerInterface> fileManager,
		std::unique_ptr<P2PNetworkInterface> networkManager);

	App(const App &) = delete;
	App & operator=(const App &) = delete;
	~App();

	void run();
	void stop();

	typedef std::unordered_map<std::string, std::vector<uint64_t>> host_id_map;

	// map: host -> it's ids
	host_id_map getPeersIds();
private:
	void listener();
};


#endif // _APP_H_INCLUDED_