#ifndef _APP_H_INCLUDED_
#define _APP_H_INCLUDED_

#include <memory>
#include <utility>
#include <unordered_map>
#include <thread>

#include "../common/p2p_network_interface.h"
#include "../common/file_manager_interface.h"

class App {
public:
	typedef std::unordered_map<std::string, std::vector<uint64_t>> host_id_map;

	typedef struct {
		uint16_t ms_port;
		uint16_t server_port;
		uint16_t file_mgr_port;
		uint16_t app_port;
		std::string main_server;
	} Settings;


	App(
		Settings settings,
		std::unique_ptr<FileManagerInterface> fileManager,
		std::unique_ptr<P2PNetworkInterface> networkManager);

	App(const App &) = delete;
	App & operator=(const App &) = delete;
	~App();

	void run();
	void stop();


	

	// map: host -> it's ids
	host_id_map getPeersIds();
private:
	void listener();


	std::unique_ptr<FileManagerInterface> fileManager;
	std::unique_ptr<P2PNetworkInterface> networkManager;

	std::thread * appThread;
	bool running;
	Settings settings;
};


#endif // _APP_H_INCLUDED_