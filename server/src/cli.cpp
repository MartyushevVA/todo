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

std::string Server::handleOption(const std::string& input){
    option opt = getOptionFromString(input);
    NodeDBTask node;
    std::cin>>node.author;
    switch(opt){
        case option::ADD:{
            std::cin>>node.title>>node.content;
            try{
                dbconnection.add(node);
                return "Added.";
            } catch(...){};
        }
        case option::REM:{
            std::cin>>node.title;
            try{
                dbconnection.rm(node);
                return "Removed.";
            } catch(...){};
        }
        case option::CHC:{
            std::cin>>node.title;
            NodeDBTask newNode;
            std::cin>>newNode.content;
            try{
                dbconnection.chc(node, newNode);
                return "Content is up to date.";
            } catch(...){};
        }
        case option::REAR:{
            std::cin>>node.title;
            NodeDBTask newNode;
            std::cin>>newNode.title;
            try{
                dbconnection.rear(node, newNode);
                return "Nodes are rearranged.";
            } catch(...){};
        }
        case option::MAD:{
            std::cin>>node.title;
            try{
                dbconnection.mad(node);
                return "Marked as done.";
            } catch(...){};
        }
        case option::SHOW:{
            try{
                auto nodes = dbconnection.getAllFrom(node.author);
                std::stringstream ss;
                for (auto node : nodes)
                    ss<<node.author<<" "<<node.title<<" "<<node.content<<" "<<node.created_at<<node.completed<<'\n';
                return ss.str();
            } catch(...){};
        }
    }
}