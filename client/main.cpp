#include "include/client.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: client <ip> <port>\n";
        return 1;
    }

    try {
        ClientApp client(argv[1], std::stoi(argv[2]));
        client.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
