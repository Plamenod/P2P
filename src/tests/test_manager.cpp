#include <memory>
#include <mutex>
#include <fstream>
#include <chrono>

#include "test_manager.h"
#include "FileManager.h"
#include "p2pnode.h"

using namespace std;

pair<bool, string> filesEqual(const std::string & left, const std::string & right) {
	ifstream l(left, ios::in | ios::binary);
	if (!l) {
		return pair<bool, string>{false, "Failed to open " + left};
	}
	ifstream r(right, ios::in | ios::binary);

	if (!r) {
		return pair<bool, string>{false, "Failed to open " + right};
	}

	l.seekg(ios::end, 0);
	r.seekg(ios::end, 0);

	if (l.tellg() != r.tellg()) {
		return pair<bool, string>{false, "File sizes differ " + to_string(l.tellg()) + " " + to_string(r.tellg())};
	}

	const int sz = 8192;
	unique_ptr<char> buffer[2] = {
		unique_ptr<char>(new char[sz]),
		unique_ptr<char>(new char[sz])
	};

	while (true) {
		// both files should have equal states after read
		if (!!l.read(buffer[0].get(), sz) != !!r.read(buffer[1].get(), sz)) {
			return pair<bool, string>{false, "Error states differ in file read"};
		}

		if (r.gcount() != l.gcount()) {
			return pair<bool, string>{false, "Failed to read same sizes from both files"};
		}

		if (memcmp(buffer[0].get(), buffer[1].get(), r.gcount()) != 0) {
			return pair<bool, string>{false, "Data missmatch in files!"};
		}
	}

	return pair<bool, string>{true, ""};
}


shared_ptr<App> createApp(int fileMgrPort, int p2pPort, int msPort, string savePath) {
	auto fileMgr = unique_ptr<FileManagerInterface>(new FileManager(fileMgrPort, savePath));
	auto node = unique_ptr<P2PNetworkInterface>(new P2PNode(msPort, p2pPort, fileMgrPort));
	App::Settings st;
	return shared_ptr<App>(new App(st, move(fileMgr), move(node)));
}

int InstanceManager::startPort = InstanceManager::msPort + 1;
int InstanceManager::startSavePath = 1;

shared_ptr<App> InstanceManager::nextNonColidingApp() {
	unique_lock<mutex> l(this->mx);
	auto app = createApp(++startPort, ++startPort, msPort, to_string(++startSavePath) + ".dat");
	this->apps.push_back(app);
	return app;
}

void InstanceManager::startMs() {
	if (!ms) {
		ms = unique_ptr<P2PMainServer>(new P2PMainServer());
	}
	this->worker = thread(&P2PMainServer::start, &*ms, msPort);
	this_thread::sleep_for(chrono::milliseconds(500));
}

void InstanceManager::stopMs() {
	ms->stop();
	this->worker.join();
	this->worker = thread();
}

void InstanceManager::clear() {
	this->apps.clear();
}

InstanceManager & InstanceManager::getInstance() {
	static InstanceManager imgr;
	return imgr;
}
