#pragma once
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include <cstdlib>
#include <iostream>

class ClientApp {
public:
    ClientApp(const std::string& ip, int port);
    void run();

private:
    int client_fd;
};
