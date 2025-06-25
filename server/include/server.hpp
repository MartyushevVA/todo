#pragma once
#include "socket_wrapper.hpp"
#include <libpq-fe.h>
#include <string>
#include <stdexcept>
#include <iostream>
#include <vector>

enum class option {
    ADD,
    REM,
    CHC,
    REAR,
    MAD
};

struct NodeDBTask {
    NodeDBTask(){};
    std::string author = "unknown";
    std::string title = "";
    std::string content = "";
};

struct NodeCLTask {
    NodeCLTask(){};
    std::string author = "unknown";
    std::string created_at = "0";
    std::string title = "";
    std::string content = "";
    std::string completed = "false";
};

class Server {
public:
    Server(int listen_port, const std::string& display_ip, int display_port, const char* db_conninfo);
    void run();

private:
    class CLI {
    public:
        static void handleOption(const std::string&);
        static option getOptionFromString(const std::string&);
        static void showAll(const std::vector<NodeCLTask>&);
    };
    class DBI {
    public:
        explicit DBI(const char* conninfo) {
            bridgeToDB = PQconnectdb(conninfo);
            if (PQstatus(bridgeToDB) != CONNECTION_OK) {
                throw std::runtime_error("Failed to connect to DB: " + std::string(PQerrorMessage(bridgeToDB)));
            }
        }
        ~DBI() {
            if (bridgeToDB) PQfinish(bridgeToDB);
        }

        void add(const NodeDBTask&);
        void rm(const NodeDBTask&);
        void chc(const NodeDBTask&, const NodeDBTask&);
        void rear(const NodeDBTask&, const NodeDBTask&);
        void mad(const NodeDBTask&);
        const std::vector<NodeCLTask>& getAllFrom(const std::string&);

    private:
        PGconn *bridgeToDB;
        int getIdByTitle(const std::string& author, const std::string& title);
    };
    DBI dbconnection;
    SocketWrapper listener_;
};