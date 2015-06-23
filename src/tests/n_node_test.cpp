#include "n_node_test.h"
#include <algorithm>

using namespace std;

NNodeTest::NNodeTest(int n, uint64_t sizePerNode)
    :nodes(n < 2 ? 2 : n), fileSize(n * sizePerNode)
{
}

TestError NNodeTest::setUp()
{
    auto & im = InstanceManager::getInstance();
    im.startMs();

    for (int c = 0; c < nodes; ++c) {
        auto app = im.getNewAppInstance();
        if (!app) {
            apps.clear();
            return TestError{ false, "Failed to initialize apps" };
        }
        apps.push_back(app);
    }

    for_each(apps.begin(), apps.end(), mem_fn(&App::run));
    std::this_thread::sleep_for(std::chrono::milliseconds(500));


    auto err = generateFile(inFile = im.getNewTmpFilename(), fileSize);
    if (!err.first) {
        return err;
    }

    auto toRemove = inFile;
    atDtor.push_back([toRemove] {
        removeFile(toRemove);
    });

    return TestError{ true, "" };
}

TestError NNodeTest::tearDown()
{
    InstanceManager::getInstance().clear();
    apps.clear();
    return TestError{ true, "" };
}

TestError NNodeTest::run()
{
    auto first = apps.front();
    
    if (!first->importFile(inFile)) {
        return TestError{ false, "Failed to import file " + inFile };
    }

    if (!first->exportFile(inFile, outFile = InstanceManager::getInstance().getNewTmpFilename())) {
        return TestError{ false, "Failed to export file " + outFile };
    }

    auto result = filesEqual(inFile, outFile);

    removeFile(outFile);

    return result;
}
