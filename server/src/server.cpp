#include "../include/server.hpp"

Server::Server(int listen_port, const char* db_conninfo) : dbconnection(db_conninfo) {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) throw std::system_error(errno, std::generic_category(), "socket failed");
    
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(listen_port);
    
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
        throw std::system_error(errno, std::generic_category(), "bind failed");
    
    if (listen(server_fd, 3) < 0)
        throw std::system_error(errno, std::generic_category(), "listen failed");
}

void Server::run() {
    while (true) {
        sockaddr_in client_address{};
        socklen_t client_addrlen = sizeof(client_address);
        int acc_socket = accept(server_fd, (struct sockaddr*)&client_address, &client_addrlen);
        if (acc_socket < 0) {
            perror("accept failed");
            continue;
        }
        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_address.sin_addr), ip_str, INET_ADDRSTRLEN);
        std::cout << "New client connected: " << ip_str << ":" << ntohs(client_address.sin_port) << std::endl;
        handleClient(acc_socket);
        close(acc_socket);
    }
}

void Server::handleClient(int client_fd) {
    std::string line;
    char c;
    ssize_t n;

    while (true) {
        line.clear();
        while (true) {
            n = recv(client_fd, &c, 1, 0);
            if (n == 0) {
                std::cout << "Client disconnected." << std::endl;
                return;
            }
            if (n < 0) {
                perror("recv error");
                return;
            }
            if (c == '\n') break;
            line += c;
        }
        std::string response = handleOption(line) + "\n";
        uint32_t response_size = response.size();
        uint32_t net_response_size = htonl(response_size); // Сетевой порядок байт

        ssize_t sent = send(client_fd, &net_response_size, sizeof(net_response_size), 0);
        if (sent <= 0) {
            perror("send length failed");
            return;
        }
        size_t total_sent = 0;
        while (total_sent < response.size()) {
            ssize_t sent = send(client_fd, response.data() + total_sent, response.size() - total_sent, 0);
            if (sent <= 0) {
                perror("send failed");
                return;
            }
            total_sent += sent;
        }
    }
}

