#ifndef _N_NODE_TEST_H_INCLUDED_
#define _N_NODE_TEST_H_INCLUDED_

#include "test_manager.h"

class NNodeTest: public TestCaseBase {
public:
    NNodeTest(int n, uint64_t sizePerNode = 1 << 20);

    TestError setUp() override;
    TestError tearDown() override;
    TestError run() override;

private:
    int nodes;
    uint64_t fileSize;

    std::string inFile, outFile;
    std::vector<InstanceManager::AppPtr> apps;
};

#endif // _N_NODE_TEST_H_INCLUDED_
