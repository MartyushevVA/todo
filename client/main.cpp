#include "include/client.hpp"
#include <iostream>

int main() {
    const char* sv_ip_c = std::getenv("SV_IP");
    const char* sv_port_c = std::getenv("SV_PORT");
    std::string sv_ip = sv_ip_c ? sv_ip_c : "127.0.0.1";
    int sv_port = sv_port_c ? std::stoi(sv_port_c) : 8080;
    try {
        ClientApp client(sv_ip, sv_port);
        client.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
