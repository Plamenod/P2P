#include <iostream>
#include <memory>

#include "file_manager_interface.h"
#include "file_manager/client/FileClient.h"

using namespace std;

int main() {
    std::unique_ptr<FileManagerInterface> fileManager(new FileClient());
    return 0;
}