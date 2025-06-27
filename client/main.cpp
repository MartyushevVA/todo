#include "include/client.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        ClientApp client(argv[1], std::stoi(argv[2]));
        client.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
