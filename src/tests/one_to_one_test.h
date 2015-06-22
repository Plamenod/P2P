#ifndef _ONE_TO_ONE_TEST_H_INCLUDED_
#define _ONE_TO_ONE_TEST_H_INCLUDED_

#include "test_manager.h"

class OneToOneTest: public TestCaseBase {
public:
    OneToOneTest();

    TestError setUp() override;
    TestError tearDown() override;
    TestError run() override;

private:
    std::string importPath, exportPath;

    InstanceManager::AppPtr left, right;
};

#endif // _ONE_TO_ONE_TEST_H_INCLUDED_
