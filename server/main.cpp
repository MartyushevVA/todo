#include "include/server.hpp"
#include <iostream>

std::string get_env_var(const std::string& key) {
    const char* val = std::getenv(key.c_str());
    if (val == nullptr) {
        throw std::runtime_error("Environment variable " + key + " not found");
    }
    return std::string(val);
}

int main() {
    try {
        std::string conninfo = "host=" + get_env_var("DB_HOST") +
                               " port=" + get_env_var("DB_PORT") +
                               " dbname=" + get_env_var("DB_NAME") +
                               " user=" + get_env_var("DB_USER") +
                               " password=" + get_env_var("DB_PASSWORD");
        Server server(std::stoi(get_env_var("PORT")), conninfo.c_str());
        server.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
