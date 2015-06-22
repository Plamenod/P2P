#include "p2p_main_server.h"

int main() {
    P2PMainServer server;
    server.start(5005);
    return 0;
}
