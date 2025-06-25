#include "include/server.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: processor <listen_port> <display_ip> <display_port>\n";
        return 1;
    }

    try {
        Server server(std::stoi(argv[1]), argv[2], std::stoi(argv[3]),
        "host=db port=5432 dbname=tasks user=lemotip password=wsxmok");
        server.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
