#include "app.h"

#include <chrono>
#include <thread>

App::App(
	Settings settings,
    std::unique_ptr<FileManagerInterface> fileManager,
    std::unique_ptr<P2PNetworkInterface> networkManager):
	settings(settings),
    fileManager(std::move(fileManager)),
    networkManager(std::move(networkManager)) {
	
	networkManager->setPorts(settings.ms_port, settings.server_port, settings.file_mgr_port);
}

void App::run() {
	running = true;
	networkManager->start(settings.main_server);
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
	// bind, listen on settings.app_port and return fileManager->myIds();
	// stop when this->running == false
}

App::host_id_map App::getPeersIds() {
	// connect to all networkManager->get_peers():settings.app_port and get their ids
	return host_id_map();
}

