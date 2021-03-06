#include "one_to_one_test.h"

using namespace std;

OneToOneTest::OneToOneTest() {}

TestError OneToOneTest::setUp() {
    auto & im = InstanceManager::getInstance();

    im.startMs();

    left = im.getNewAppInstance();
    right = im.getNewAppInstance();

    left->run();
    right->run();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));


    importPath = "one-to-one-import.dat";
    exportPath = "one-to-one-export.dat";

    auto err = generateFile(importPath, (1 << 20) * 2);
    if (!err.first) {
        return err;
    }

    string toRemove = this->importPath;
    atDtor.push_back([toRemove] {
        removeFile(toRemove);
    });

    return TestError{true, ""};
}

TestError OneToOneTest::tearDown() {
    InstanceManager::getInstance().clear();
    return TestError{true, ""};
}

TestError OneToOneTest::run() {
    if (!left->importFile(importPath)) {
        return TestError{false, ""};
    }

    if (!left->exportFile(importPath, exportPath)) {
        return TestError{false, ""};
    }

    auto result = filesEqual(importPath, exportPath);

    removeFile(exportPath);

    return result;
}
