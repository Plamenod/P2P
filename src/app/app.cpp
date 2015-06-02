#include "app.h"

#include <chrono>
#include <thread>

App::App(
    std::unique_ptr<FileManagerInterface> fileManager,
    std::unique_ptr<P2PNetworkInterface> networkManager):
    fileManager(std::move(fileManager)),
    networkManager(std::move(networkManager)) {}

void App::run() {
	running = true;
	//networkManager->run();
	fileManager->run();
	appThread = new std::thread(std::bind(&App::listener, this));
}

void App::stop() {
	running = false;
	appThread->join();
	delete appThread;
}

App::~App() {
	stop();
}

void App::listener() {
	// bind, listen and return fileManager->myIds();
	// stop when this->running == false
}

App::host_id_map App::getPeersIds() {
	// connect to all networkManager->get_peers() and get their ids
	return host_id_map();
}

