#include <iostream>

#include "test_manager.h"
#include "one_to_one_test.h"

using namespace std;

int main() {
    vector<shared_ptr<TestCaseBase>> tests;

    string f1 = "D:/dev/1.pdf";
    string f2 = "D:/dev/2.pdf";
    tests.emplace_back(new OneToOneTest(f1, f2));
	

    vector<TestError> errors;

    for (auto & test : tests) {
        test->setUp();
        auto error = test->run();
        if (error.first) {
            errors.push_back(error);
        }
        test->tearDown();
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
