#ifndef _APP_H_INCLUDED_
#define _APP_H_INCLUDED_

#include <memory>
#include <utility>

#include "../common/p2p_network_interface.h"
#include "../common/file_manager_interface.h"

class App {
	std::unique_ptr<FileManagerInterface> fileManager;
	std::unique_ptr<P2PNetworkInterface> networkManager;
public:
	App(
		std::unique_ptr<FileManagerInterface> fileManager,
		std::unique_ptr<P2PNetworkInterface> networkManager);

	void run();
};


#endif // _APP_H_INCLUDED_