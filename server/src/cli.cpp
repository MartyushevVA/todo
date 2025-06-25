#include "../include/server.hpp"

option Server::CLI::getOptionFromString(const std::string& str) {    
    if (str == "ADD") return option::ADD;
    if (str == "REM") return option::REM;
    if (str == "CHC") return option::CHC;
    if (str == "REAR") return option::REAR;
    if (str == "MAD") return option::MAD;
    throw std::invalid_argument("Unknown option");
}

void Server::CLI::handleOption(const std::string& input){
    option opt = getOptionFromString(input);
    NodeDBTask node;
    std::cin>>node.author;
    switch(opt){
        case option::ADD:{
            std::cin>>node.title>>node.content;
            try{
                DBI::add(node);
            } catch(...){};
        }
        case option::REM:{
            std::cin>>node.title;
            try{
                DBI::rm(node);
            } catch(...){};
        }
        case option::CHC:{
            std::cin>>node.title;
            NodeDBTask newNode;
            std::cin>>newNode.content;
            try{
                DBI::chc(node, newNode);
            } catch(...){};
        }
        case option::REAR:{
            std::cin>>node.title;
            NodeDBTask newNode;
            std::cin>>newNode.title;
            try{
                DBI::rear(node, newNode);
            } catch(...){};
        }
        case option::MAD:{
            std::cin>>node.title;
            try{
                DBI::mad(node);
            } catch(...){};
        }
    }
    CLI::showAll(DBI::getAllFrom(node.author));
}

void Server::CLI::showAll(const std::vector<NodeCLTask>& nodes){
    for (auto node : nodes)
        std::cout<<node.author<<node.title<<node.content<<node.created_at<<node.completed<<'\n';
}