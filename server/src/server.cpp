#include "../include/server.hpp"

Server::Server(int listen_port, const char* db_conninfo) : dbconnection(db_conninfo) {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) throw std::system_error(errno, std::generic_category(), "socket failed");
    
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(listen_port);
    
    if (bind(server_fd, reinterpret_cast<struct sockaddr*>(&address), sizeof(address)) < 0)
        throw std::system_error(errno, std::generic_category(), "bind failed");
    
    if (listen(server_fd, 3) < 0)
        throw std::system_error(errno, std::generic_category(), "listen failed");
}

void Server::run() {
    while (true) {
        sockaddr_in client_address{};
        socklen_t client_addrlen = sizeof(client_address);
        int acc_socket = accept(server_fd, reinterpret_cast<struct sockaddr*>(&client_address), &client_addrlen);
        if (acc_socket < 0) {
            perror("accept failed");
            continue;
        }
        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_address.sin_addr), ip_str, INET_ADDRSTRLEN);
        std::cout << "New client connected: " << ip_str << ":" << ntohs(client_address.sin_port) << std::endl;
        try{
            handleClient(acc_socket);
        } catch(...){};
        close(acc_socket);
    }
}

std::string receiveData(int fd){
    std::string line;
    char c;
    while (true) {
        ssize_t n = recv(fd, &c, 1, 0);
        if (n <= 0)
            throw std::runtime_error("Disconnected.");
        if (c == '\n') break;
        line += c;
    }
    return line;
}

void sendData(int fd, const std::string& data){
    uint32_t data_size = data.size();
    uint32_t net_data_size = htonl(data_size);
    ssize_t sent = send(fd, &net_data_size, sizeof(net_data_size), 0);
    if (sent <= 0) 
        throw std::runtime_error("Disconnected.");
    size_t total_sent = 0;
    while (total_sent < data.size()) {
        sent = send(fd, data.data() + total_sent, data.size() - total_sent, 0);
        if (sent <= 0) 
            throw std::runtime_error("Disconnected.");
        total_sent += sent;
    }
}

void Server::handleClient(int client_fd) {
    bool isAuthorized = false;
    std::string username = "";
    std::string line;
    sendData(client_fd, "Enter [login passwd] please: ");
    while (!isAuthorized) {
        line = receiveData(client_fd);
        authstatus Status = tryAuth(line);
        switch (Status){
            case authstatus::NO:{
                sendData(client_fd, "Confirm signing up: ");
                std::string confirm = receiveData(client_fd);
                if (confirm == "Yes"){
                    newUser(line);
                    isAuthorized = true;
                    username = getRegData(line).first;
                    sendData(client_fd, "Signed up successfully.\nEnter query: ");
                }
                else
                    sendData(client_fd, "Oops! Try again: ");
                break;
            }
            case authstatus::PRESENT:{
                isAuthorized = true;
                username = getRegData(line).first;
                sendData(client_fd, "Signed in successfully.\nEnter query: ");
                break;
            }
            default:{
                sendData(client_fd, "Wrong password. Try again: ");
                break;
            }
        }
    }
    while(true){
        line = receiveData(client_fd);
        sendData(client_fd, handleOption(username, line)+ "\nEnter query: ");
    }
}

