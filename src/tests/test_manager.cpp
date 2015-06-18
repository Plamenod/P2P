#include <memory>
#include <mutex>
#include <fstream>
#include <chrono>

#include "test_manager.h"
#include "FileManager.h"
#include "p2pnode.h"

using namespace std;

TestError filesEqual(const std::string & left, const std::string & right) {
	ifstream l(left, ios::in | ios::binary);
	if (!l) {
		return TestError{false, "Failed to open " + left};
	}
	ifstream r(right, ios::in | ios::binary);

	if (!r) {
		return TestError{false, "Failed to open " + right};
	}

	l.seekg(ios::end, 0);
	r.seekg(ios::end, 0);

	if (l.tellg() != r.tellg()) {
		return TestError{false, "File sizes differ " + to_string(l.tellg()) + " " + to_string(r.tellg())};
	}

	const int sz = 8192;
	unique_ptr<char> buffer[2] = {
		unique_ptr<char>(new char[sz]),
		unique_ptr<char>(new char[sz])
	};

	while (l && r) {
		// both files should have equal states after read
		if (!!l.read(buffer[0].get(), sz) != !!r.read(buffer[1].get(), sz)) {
			return TestError{false, "Error states differ in file read"};
		}

		if (r.gcount() != l.gcount()) {
			return TestError{false, "Failed to read same sizes from both files"};
		}

		if (memcmp(buffer[0].get(), buffer[1].get(), r.gcount()) != 0) {
			return TestError{false, "Data missmatch in files!"};
		}
	}

	return TestError{true, ""};
}


unique_ptr<App> createApp(int fileMgrPort, int p2pPort, int msPort, string savePath) {
	auto fileMgr = unique_ptr<FileManagerInterface>(new FileManager(fileMgrPort, savePath));
	auto node = unique_ptr<P2PNetworkInterface>(new P2PNode(msPort, p2pPort, fileMgrPort));
	App::Settings st;
	st.main_server = "127.0.0.1";
	st.ms_port = msPort;
	st.file_mgr_port = fileMgrPort;
	st.server_port = p2pPort;
	return unique_ptr<App>(new App(st, move(fileMgr), move(node)));
}

int InstanceManager::startPort = InstanceManager::msPort + 1;
int InstanceManager::startSavePath = 1;

shared_ptr<App> InstanceManager::nextNonColidingApp() {
	unique_lock<mutex> l(this->mx);
	auto fname = to_string(++startSavePath) + ".dat";

	auto app = createApp(++startPort, ++startPort, msPort, fname);
	shared_ptr<App> appPtr(app.release(), [fname](App * app) {
		delete app;
		remove(fname.c_str());
	});

	this->apps.push_back(appPtr);
	return appPtr;
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
	if (this->worker.joinable()) {
		this->worker.join();
	}
	this->worker = thread();
}

void InstanceManager::clear() {
	this->apps.clear();
}

InstanceManager::~InstanceManager() {
	this->clear();
	this->stopMs();
}

InstanceManager & InstanceManager::getInstance() {
	static InstanceManager imgr;
	return imgr;
}
