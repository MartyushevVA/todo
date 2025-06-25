#include "../include/server.hpp"

void Server::DBI::add(const NodeDBTask& node){
    const char* query = "INSERT INTO tasks (author, title, content, completed) VALUES ($1, $2, $3, $4)";
    const char* paramValues[4] = {node.author.c_str(), node.title.c_str(), node.content.c_str(), "false" };
    int paramLengths[4] = { (int)node.author.size(), (int)node.title.size(), (int)node.content.size(), 5 };
    int paramFormats[4] = { 0, 0, 0, 0 }; // 0 = текстовый формат
    PGresult* res = PQexecParams(
        bridgeToDB,
        query,
        4,             // Количество параметров
        nullptr,       // Типы параметров (null, пусть Postgres сам определит)
        paramValues,   // Значения параметров
        paramLengths,  // Длины параметров
        paramFormats,  // Форматы параметров (0 = текст)
        0              // Формат результата (0 = текст)
    );
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Insert failed: " << PQerrorMessage(bridgeToDB) << std::endl;
        PQclear(res);
        return;
    }
    std::cout << "Task added successfully!" << std::endl;
    PQclear(res);
}

void Server::DBI::rm(const NodeDBTask& node){
    const char* query = "DELETE FROM tasks WHERE author = $1 AND title = $2";
    const char* paramValues[2] = {node.author.c_str(), node.title.c_str() };
    int paramLengths[2] = { (int)node.author.size(), (int)node.title.size() };
    int paramFormats[2] = { 0, 0 }; // 0 = текстовый формат
    PGresult* res = PQexecParams(
        bridgeToDB,
        query,
        2,             // Количество параметров
        nullptr,       // Типы параметров (null, пусть Postgres сам определит)
        paramValues,   // Значения параметров
        paramLengths,  // Длины параметров
        paramFormats,  // Форматы параметров (0 = текст)
        0              // Формат результата (0 = текст)
    );
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Removing failed: " << PQerrorMessage(bridgeToDB) << std::endl;
        PQclear(res);
        return;
    }
    std::cout << "Task removed successfully!" << std::endl;
    PQclear(res);
}

void Server::DBI::chc(const NodeDBTask& node1, const NodeDBTask& node2){
    const char* query = "UPDATE tasks SET content = $1 WHERE author = $2 AND title = $3";
    const char* paramValues[3] = {node2.content.c_str(), node1.author.c_str(), node1.title.c_str()};
    int paramLengths[3] = { (int)node2.content.c_str(), (int)node1.author.c_str(), (int)node1.title.c_str() };
    int paramFormats[3] = { 0, 0, 0 };
    PGresult* res = PQexecParams(
        bridgeToDB,
        query,
        3,
        nullptr,
        paramValues,
        paramLengths,
        paramFormats,
        0
    );
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Update failed: " << PQerrorMessage(bridgeToDB) << std::endl;
        PQclear(res);
        return;
    }
    std::cout << "Task updated successfully!" << std::endl;
    PQclear(res);
}

void Server::DBI::rear(const NodeDBTask& node1, const NodeDBTask& node2){
    int id1 = getIdByTitle(node1.author, node1.title);
    int id2 = getIdByTitle(node1.author, node2.title);
    if (id1 == -1 || id2 == -1) {
        std::cerr << "Error: One of the records was not found." << std::endl;
        return;
    }

    PGresult* res = PQexec(bridgeToDB, "BEGIN;");
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "BEGIN failed: " << PQerrorMessage(bridgeToDB) << std::endl;
        PQclear(res);
        return;
    }
    PQclear(res);

    try {
        const char* tempQuery = "UPDATE tasks SET id = -1 WHERE id = $1";
        std::string id1Str = std::to_string(id1);
        const char* tempValues[1] = { id1Str.c_str() };
        res = PQexecParams(bridgeToDB, tempQuery, 1, nullptr, tempValues, nullptr, nullptr, 0);
        if (PQresultStatus(res) != PGRES_COMMAND_OK) throw std::runtime_error("smt went wrong");
        PQclear(res);

        const char* updateQuery = "UPDATE tasks SET id = $1 WHERE id = $2";
        std::string id2Str = std::to_string(id2);
        const char* updateValues1[2] = { id1Str.c_str(), id2Str.c_str() };
        res = PQexecParams(bridgeToDB, updateQuery, 2, nullptr, updateValues1, nullptr, nullptr, 0);
        if (PQresultStatus(res) != PGRES_COMMAND_OK) throw std::runtime_error("smt went wrong");
        PQclear(res);

        const char* updateValues2[2] = { id2Str.c_str(), "-1" };
        res = PQexecParams(bridgeToDB, updateQuery, 2, nullptr, updateValues2, nullptr, nullptr, 0);
        if (PQresultStatus(res) != PGRES_COMMAND_OK) throw std::runtime_error("smt went wrong");
        PQclear(res);

        res = PQexec(bridgeToDB, "COMMIT;");
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            std::cerr << "COMMIT failed: " << PQerrorMessage(bridgeToDB) << std::endl;
        } else {
            std::cout << "IDs swapped successfully!" << std::endl;
        }
        PQclear(res);
        return;
    } catch(std::runtime_error& e){
        std::cerr << "Swap failed: " << PQerrorMessage(bridgeToDB) << std::endl;
        PQclear(res);
        PGresult* rb = PQexec(bridgeToDB, "ROLLBACK;");
        PQclear(rb);
    }
}

void Server::DBI::mad(const NodeDBTask& node){
    const char* query = "UPDATE tasks SET completed = true WHERE author = $1 AND title = $2";
    const char* paramValues[2] = {node.author.c_str(), node.title.c_str()};
    int paramLengths[2] = {(int)node.author.size(), (int)node.title.size()};
    int paramFormats[2] = {0, 0};
    PGresult* res = PQexecParams(
        bridgeToDB,
        query,
        2,
        nullptr,
        paramValues,
        paramLengths,
        paramFormats,
        0
    );
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Marking failed: " << PQerrorMessage(bridgeToDB) << std::endl;
        PQclear(res);
        return;
    }
    std::cout << "Task marked as done successfully!" << std::endl;
    PQclear(res);
}

int Server::DBI::getIdByTitle(const std::string& author, const std::string& title) {
    const char* query = "SELECT id FROM tasks WHERE author = $1 and title = $2 LIMIT 1";
    const char* paramValues[2] = { author.c_str(), title.c_str() };
    PGresult* res = PQexecParams(bridgeToDB, query, 2, nullptr, paramValues, nullptr, nullptr, 0);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Select failed: " << PQerrorMessage(bridgeToDB) << std::endl;
        PQclear(res);
        return -1;
    }
    if (PQntuples(res) == 0) {
        std::cerr << "No record found for value: " << title << std::endl;
        PQclear(res);
        return -1;
    }
    int id = std::stoi(PQgetvalue(res, 0, 0));
    PQclear(res);
    return id;
}

const std::vector<NodeCLTask>& Server::DBI::getAllFrom(const std::string& author){
    std::vector<NodeCLTask> nodes;

    const char* query = "SELECT author, title, content, created_at, completed FROM tasks WHERE author = $1";
    const char* paramValues[1] = {author.c_str()};
    int paramLengths[1] = {(int)author.size()};
    int paramFormats[1] = {0};
    PGresult* res = PQexecParams(
        bridgeToDB,
        query,
        1,
        nullptr,
        paramValues,
        paramLengths,
        paramFormats,
        0
    );

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Select failed: " << PQerrorMessage(bridgeToDB) << std::endl;
        PQclear(res);
        return nodes;
    }
    int rows = PQntuples(res);
    for (int i = 0; i < rows; i++) {
        NodeCLTask node;
        node.author = PQgetvalue(res, i, 0);
        node.title = PQgetvalue(res, i, 1);
        node.content = PQgetvalue(res, i, 2);
        node.created_at = PQgetvalue(res, i, 3);
        node.completed = PQgetvalue(res, i, 4);
        nodes.push_back(std::move(node));
    }
    PQclear(res);
    return nodes;
}