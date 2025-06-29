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

std::string Server::handleOption(const std::string& input) {
    std::istringstream iss(input);
    std::string opt_str;
    iss >> opt_str;
    option opt = getOptionFromString(opt_str);

    NodeDBTask node;
    switch(opt) {
        case option::ADD: {
            iss >> node.author >> node.title;
            std::getline(iss, node.content); // считываем остаток строки как content (с возможным пробелом)
            node.content.erase(0, node.content.find_first_not_of(' ')); // убираем ведущие пробелы
            try {
                dbconnection.add(node);
                return "Added.";
            } catch (...) {
                return "Add failed.";
            }
        }
        case option::REM: {
            iss >> node.author >> node.title;
            try {
                dbconnection.rm(node);
                return "Removed.";
            } catch (...) {
                return "Remove failed.";
            }
        }
        case option::CHC: {
            iss >> node.author >> node.title;
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
            iss >> node.author >> node.title;
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
            iss >> node.author >> node.title;
            try {
                dbconnection.mad(node);
                return "Marked done.";
            } catch (...) {
                return "Mark failed.";
            }
        }
        case option::SHOW: {
            iss >> node.author;
            try {
                auto nodes = dbconnection.getAllFrom(node.author);
                std::stringstream ss;
                for (auto& node : nodes) {
                    ss << "\n" << node.author << " " << node.title << " " << node.content << " " << node.created_at << " " << node.completed;
                }
                return ss.str();
            } catch (...) {
                return "Show failed.";
            }
        }
        default:
            return "Unknown command";
    }
}
