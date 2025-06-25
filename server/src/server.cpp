#include "../include/server.hpp"

Server::Server(int listen_port, const std::string& display_ip, int display_port, const char* db_conninfo)
    : dbconnection(db_conninfo), listener_(SocketWrapper::listen_on(listen_port)) {
    if (!listener_.is_valid()) throw std::runtime_error("Failed to bind processor socket");
}

void Server::run() {
    SocketWrapper client = listener_.accept_connection();
    if (!client.is_valid()) {
        std::cerr << "Failed to accept client\n";
        return;
    }
    std::string input;
    while (client.recv_string(input)) {
        CLI::handleOption(input);
    }
}