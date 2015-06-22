#include <iostream>

#include "test_manager.h"
#include "one_to_one_test.h"

using namespace std;

int main() {

    vector<shared_ptr<TestCaseBase>> tests;

    string f1 = "D:/dev/1.pdf";
    string f2 = "D:/dev/2.pdf";
    tests.emplace_back(new OneToOneTest());


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
