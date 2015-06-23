#include <iostream>

#include "test_manager.h"
#include "one_to_one_test.h"
#include "n_node_test.h"

using namespace std;

int main() {

    vector<shared_ptr<TestCaseBase>> tests;

    tests.emplace_back(new OneToOneTest());
    tests.emplace_back(new NNodeTest(5));
    tests.emplace_back(new NNodeTest(5, (1 << 20) * 10));

    vector<TestError> errors;

    for (auto & test : tests) {
        TestError err;

        err = test->setUp();
        if (err.first) {
            err = test->run();
            if (!err.first) {
                errors.push_back(err);
            }
        } else {
            errors.push_back(err);
        }


        err = test->tearDown();
        if (!err.first) {
            errors.push_back(err);
        }
    }

    cout << "\n\n------------------------------------------\n"
        << "Tests ran: " << tests.size() << "\n"
        << "Test passed: " << (tests.size() - errors.size()) << "\n"
        << "Tests failed: " << errors.size() << "\n\n";

    for (auto & err : errors) {
        cout << err.second << "\n";
    }

    return 0;
}
