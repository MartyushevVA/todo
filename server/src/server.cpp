#include "../include/server.hpp"

Server::Server(int listen_port, const char* db_conninfo) : dbconnection(db_conninfo) {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) throw std::system_error(errno, std::generic_category(), "socket failed");
    sockaddr_in address;
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(listen_port);
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
        throw std::system_error(errno, std::generic_category(), "bind failed");
    if (listen(server_fd, 3) < 0)
        throw std::system_error(errno, std::generic_category(), "listen failed");
    sockaddr_in client_address;
    socklen_t client_addrlen = sizeof(client_address);
    acc_socket = accept(server_fd, (struct sockaddr*)&client_address, &client_addrlen);
    if (acc_socket < 0)
        throw std::system_error(errno, std::generic_category(), "accept failed");
}

void Server::run() {
    while (true) {
        sockaddr_in address;
        socklen_t addrlen = sizeof(address);
        int client_fd = accept(server_fd, (struct sockaddr*)&address, &addrlen);
        if (client_fd < 0) {
            std::cerr << "Accept failed\n";
            continue;
        }
        handleClient(client_fd);
        close(client_fd);
    }
}

void Server::handleClient(int client_fd) {
    char input[1024];
    ssize_t bytes_read;
    while ((bytes_read = read(client_fd, input, sizeof(input) - 1)) > 0) {
        input[bytes_read] = '\0';
        std::string response = handleOption(std::string(input));
        send(client_fd, response.c_str(), response.size(), 0);
    }
}