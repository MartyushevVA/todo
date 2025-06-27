#include "../include/client.hpp"

ClientApp::ClientApp(const std::string& ip, int port) {
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    std::string port_str = std::to_string(port);
    int err = getaddrinfo(ip.c_str(), port_str.c_str(), &hints, &res);
    if (err != 0) {
        std::cerr << "getaddrinfo: " << gai_strerror(err) << std::endl;
        exit(EXIT_FAILURE);
    }
    if (connect(client_fd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("Connection failed");
        freeaddrinfo(res);
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(res);
}

void ClientApp::run() {
    std::string input;
    char buffer[1024] = {0};
    while (true) {
        std::cout << "Enter message: ";
        std::getline(std::cin, input);
        if (input == "exit")
            break;
        ssize_t sent = send(client_fd, input.c_str(), input.length(), 0);
        if (sent < 0) {
            perror("send failed");
            break;
        }
        ssize_t bytes_received = read(client_fd, buffer, sizeof(buffer) - 1);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            std::cout << "Server response: " << buffer << std::endl;
        } else if (bytes_received == 0) {
            std::cout << "Server closed connection.\n";
            break;
        } else {
            perror("read failed");
            break;
        }
        memset(buffer, 0, sizeof(buffer));
    }
    close(client_fd);
}