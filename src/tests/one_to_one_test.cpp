#include "one_to_one_test.h"

using namespace std;

OneToOneTest::OneToOneTest(std::string import, std::string export)
    : importPath(move(import)), exportPath(move(export))
{
}

void OneToOneTest::setUp()
{
    auto & im = InstanceManager::getInstance();

    im.startMs();

    left = im.nextNonColidingApp();
    right = im.nextNonColidingApp();

    left->run();
    right->run();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void OneToOneTest::tearDown()
{
    InstanceManager::getInstance().clear();
}

TestError OneToOneTest::run()
{
    if (!left->importFile(importPath)) {
        return TestError{ false, "" };
    }

    if (!left->exportFile(importPath, exportPath)) {
        return TestError{ false, "" };
    }

    return filesEqual(importPath, exportPath);
}
