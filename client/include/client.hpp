#pragma once
#include "socket_wrapper.hpp"

class ClientApp {
public:
    ClientApp(const std::string& ip, int port);
    void run();

private:
    SocketWrapper socket_;
};
