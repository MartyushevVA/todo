#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pqxx/pqxx>
#include <string>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <cstring>
#include <sstream>
#include <memory>
#include <cstdlib>

enum class option {
    ADD,
    REM,
    CHC,
    REAR,
    MAD,
    SHOW
};

enum class authstatus {
    PRESENT,
    NO,
    WRONG
};

struct NodeDBTask {
    explicit NodeDBTask(const std::string& name) : author(name){};
    NodeDBTask() = default;
    std::string author = "unknown";
    std::string title = "";
    std::string content = "";
};

struct NodeCLTask {
    NodeCLTask() = default;
    std::string author = "unknown";
    std::string created_at = "0";
    std::string title = "";
    std::string content = "";
    std::string completed = "false";
};

class DBI {
public:
    explicit DBI(const char* conninfo) {
        try {
            bridgeToDB = std::make_shared<pqxx::connection>(conninfo);
            if (!bridgeToDB->is_open()) {
                throw std::runtime_error("Failed to connect to DB");
            }
        } catch (const std::exception& e) {
            throw std::runtime_error("Database connection error: " + std::string(e.what()));
        }
    }

    authstatus check(const std::string&, const std::string&);
    void createUser(const std::string&, const std::string&);

    void add(const NodeDBTask&);
    void rm(const NodeDBTask&);
    void chc(const NodeDBTask&, const NodeDBTask&);
    void rear(const NodeDBTask&, const NodeDBTask&);
    void mad(const NodeDBTask&);
    const std::vector<NodeCLTask>& getAllFrom(const std::string&);

private:
    std::shared_ptr<pqxx::connection> bridgeToDB;
    int getIdByTitle(const std::string& author, const std::string& title);
    std::vector<NodeCLTask> nodes;
};

class Server {
public:
    Server(int listen_port, const char* db_conninfo);
    void run();

private:
    authstatus tryAuth(const std::string&);
    void newUser(const std::string&);
    std::string handleOption(const std::string&, const std::string&);
    void handleClient(int);
    static option getOptionFromString(const std::string&);
    static std::pair<std::string, std::string> getRegData(const std::string&);
    

    int server_fd;
    DBI dbconnection;
};
