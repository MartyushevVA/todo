#include "../include/client.hpp"

void log_connected_server(int client_fd) {
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    if (getpeername(client_fd, (struct sockaddr*)&addr, &addr_len) == -1) {
        perror("getpeername failed");
        return;
    }
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(addr.sin_addr), ip_str, sizeof(ip_str));
    std::cout << "Connected to server: " << ip_str << ":" << ntohs(addr.sin_port) << std::endl;
}

void log_client_ip(int client_fd) {
    struct sockaddr_in local_addr;
    socklen_t addr_len = sizeof(local_addr);
    if (getsockname(client_fd, (struct sockaddr*)&local_addr, &addr_len) == -1) {
        perror("getsockname failed");
        return;
    }
    char ip_str[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &(local_addr.sin_addr), ip_str, sizeof(ip_str)) == nullptr) {
        perror("inet_ntop failed");
        return;
    }
    std::cout << "Client local IP: " << ip_str << ":" << ntohs(local_addr.sin_port) << std::endl;
}


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
    log_connected_server(client_fd);
    log_client_ip(client_fd);
}

void ClientApp::run() {
    std::string input;
    char buffer[1024];
    
    while (true) {
        std::cout << "Enter message: " << std::flush;
        if (!std::getline(std::cin, input)) break;
        if (input == "exit")
            break;

        std::string to_send = input + "\n";
        size_t total_sent = 0;
        while (total_sent < to_send.size()) {
            ssize_t sent = send(client_fd, to_send.data() + total_sent, to_send.size() - total_sent, 0);
            if (sent <= 0) {
                perror("send failed");
                return;
            }
            total_sent += sent;
        }
        
        uint32_t net_response_size;
        ssize_t n = recv(client_fd, &net_response_size, sizeof(net_response_size), 0);
        if (n <= 0) {
            std::cout << "Failed to receive message length or server closed connection." << std::endl;
            return;
        }
        uint32_t response_size = ntohl(net_response_size);
        
        std::string server_response(response_size, '\0');
        size_t total_received = 0;
        while (total_received < response_size) {
            n = recv(client_fd, &server_response[total_received], response_size - total_received, 0);
            if (n <= 0) {
                std::cout << "Failed to receive message or server closed connection." << std::endl;
                return;
            }
            total_received += n;
        }
        std::cout << "Server response: " << server_response << std::endl;
    }
    close(client_fd);
}