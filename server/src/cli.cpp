#include "../include/server.hpp"

option Server::getOptionFromString(const std::string& str) {    
    if (str == "ADD") return option::ADD;
    if (str == "REM") return option::REM;
    if (str == "CHC") return option::CHC;
    if (str == "REAR") return option::REAR;
    if (str == "MAD") return option::MAD;
    if (str == "SHOW") return option::SHOW;
    throw std::invalid_argument("Unknown option");
}

std::pair<std::string, std::string> Server::getRegData(const std::string& line){
    std::istringstream iss(line);
    std::string login, passwd;
    iss >> login >> passwd;
    return std::pair(login, passwd);
}

authstatus Server::tryAuth(const std::string& line){
    auto node = getRegData(line);
    return dbconnection.check(node.first, node.second);
}

void Server::newUser(const std::string& line){
    std::istringstream iss(line);
    std::string login, passwd;
    iss >> login >> passwd;
    dbconnection.createUser(login, passwd);
}

std::string Server::handleOption(const std::string& username, const std::string& input) {
    std::istringstream iss(input);
    std::string opt_str;
    iss >> opt_str;
    option opt = getOptionFromString(opt_str);
    NodeDBTask node(username);
    switch(opt) {
        case option::ADD: {
            iss >> node.title;
            std::getline(iss, node.content);
            node.content.erase(0, node.content.find_first_not_of(' '));
            try {
                dbconnection.add(node);
                return "Added.";
            } catch (...) {
                return "Add failed.";
            }
        }
        case option::REM: {
            iss >> node.title;
            try {
                dbconnection.rm(node);
                return "Removed.";
            } catch (...) {
                return "Remove failed.";
            }
        }
        case option::CHC: {
            iss >> node.title;
            NodeDBTask newNode;
            std::getline(iss, newNode.content);
            newNode.content.erase(0, newNode.content.find_first_not_of(' '));
            try {
                dbconnection.chc(node, newNode);
                return "Content updated.";
            } catch (...) {
                return "Update failed.";
            }
        }
        case option::REAR: {
            iss >> node.title;
            NodeDBTask newNode;
            iss >> newNode.title;
            try {
                dbconnection.rear(node, newNode);
                return "Rearranged.";
            } catch (...) {
                return "Rearrange failed.";
            }
        }
        case option::MAD: {
            iss >> node.title;
            try {
                dbconnection.mad(node);
                return "Marking done.";
            } catch (...) {
                return "Mark failed.";
            }
        }
        case option::SHOW: {
            try {
                auto nodes = dbconnection.getAllFrom(node.author);
                std::stringstream ss;
                for (auto& node : nodes) {
                    ss << "\n" << node.author << " " << node.title << " " << node.content << " " << node.created_at << " " << node.completed;
                }
                return ss.str().substr(1);
            } catch (...) {
                return "Show failed.";
            }
        }
        default:
            return "Unknown command";
    }
}
