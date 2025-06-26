#include "../include/server.hpp"

void Server::run() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
        throw std::system_error();

    sockaddr_in address;
    int addrlen = sizeof(address);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Слушать на всех интерфейсах
    address.sin_port = htons(8080);       // Порт сервера

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
        throw std::system_error();
    if (listen(server_fd, 3) < 0)
        throw std::system_error();

    int new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    if (new_socket < 0) 
        throw std::system_error();

    char input[1024] = {0};
    while (read(new_socket, input, 1024)){
        std::string response = handleOption(input);
        send(new_socket, response.c_str(), strlen(response.c_str()), 0);
    }
    close(new_socket);
    close(server_fd);
}