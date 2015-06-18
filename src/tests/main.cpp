#include <iostream>

#include "test_manager.h"

using namespace std;

int main() {
	auto & im = InstanceManager::getInstance();

	im.startMs();

	auto l = im.nextNonColidingApp();
	auto r = im.nextNonColidingApp();

	l->run();
	r->run();

	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	string f1 = "D:/dev/1.pdf";
	string f2 = "D:/dev/2.pdf";

	if (!l->importFile(f1)) {
		cerr << "Failed to import file";
		return 0;
	}

	if (!l->exportFile(f1, f2)) {
		cerr << "Failed to export file";
		return 0;
	}

	auto res = filesEqual(f1, f2);

	if (!res.first) {
		cerr << res.second;
	}

	return 0;
}
