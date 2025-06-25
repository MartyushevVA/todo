#include "../include/client.hpp"
#include <iostream>

ClientApp::ClientApp(const std::string& ip, int port)
    : socket_(SocketWrapper::connect_to(ip, port)) {
    if (!socket_.is_valid()) {
        throw std::runtime_error("Failed to connect to server");
    }
}

void ClientApp::run() {
    std::string input;
    while (std::cout << "> ", std::getline(std::cin, input)) {
        if (!socket_.send_string(input)) break;
        std::string response;
        if (!socket_.recv_string(response)) break;
        std::cout << "Server acknowledged: " << response << "\n";
    }
}
