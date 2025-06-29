#include "../include/server.hpp"

authstatus DBI::check(const std::string& login, const std::string& passwd){
    pqxx::work txn(*bridgeToDB);
    pqxx::result res = txn.exec_params(
        "SELECT passwd FROM users WHERE login = ($1)",
        login
    );
    txn.commit();
    if (res.empty())
        return authstatus::NO;
    std::string passwd_ = res[0][0].as<std::string>();
    if (passwd == passwd_) return authstatus::PRESENT;
    return authstatus::WRONG;
}

void DBI::createUser(const std::string& login, const std::string& passwd){
    pqxx::work txn(*bridgeToDB);
    txn.exec_params(
        "INSERT INTO users (login, passwd) VALUES ($1, $2)",
        login, passwd
    );
    txn.commit();
}

void DBI::add(const NodeDBTask& node) {
    pqxx::work txn(*bridgeToDB);
    auto res = txn.exec_params(
        "INSERT INTO tasks (author, title, content, completed) VALUES ($1, $2, $3, $4)",
        node.author, node.title, node.content, false
    );
    txn.commit();
    if (!res.affected_rows()) throw std::runtime_error("");
}

void DBI::rm(const NodeDBTask& node) {
    pqxx::work txn(*bridgeToDB);
    auto res = txn.exec_params(
        "DELETE FROM tasks WHERE author = $1 AND title = $2",
        node.author, node.title
    );
    txn.commit();
    if (!res.affected_rows()) throw std::runtime_error("");
}

void DBI::chc(const NodeDBTask& node1, const NodeDBTask& node2) {
    pqxx::work txn(*bridgeToDB);
    auto res = txn.exec_params(
        "UPDATE tasks SET content = $1 WHERE author = $2 AND title = $3",
        node2.content, node1.author, node1.title
    );
    txn.commit();
    if (!res.affected_rows()) throw std::runtime_error("");
}

void DBI::rear(const NodeDBTask& node1, const NodeDBTask& node2) {
    int id1 = getIdByTitle(node1.author, node1.title);
    int id2 = getIdByTitle(node1.author, node2.title);
    if (id1 == -1 || id2 == -1)
        return;

    pqxx::work txn(*bridgeToDB);

    txn.exec_params("UPDATE tasks SET id = -1 WHERE id = $1", id1);
    txn.exec_params("UPDATE tasks SET id = $1 WHERE id = $2", id1, id2);
    auto res = txn.exec_params("UPDATE tasks SET id = $1 WHERE id = -1", id2);

    txn.commit();
    if (!res.affected_rows()) throw std::runtime_error("");
}

void DBI::mad(const NodeDBTask& node) {
    pqxx::work txn(*bridgeToDB);
    pqxx::result res = txn.exec_params(
        "UPDATE tasks SET completed = true WHERE author = $1 AND title = $2",
        node.author, node.title
    );
    txn.commit();
    if (!res.affected_rows()) throw std::runtime_error("");
}

int DBI::getIdByTitle(const std::string& author, const std::string& title) {
    pqxx::work txn(*bridgeToDB);
    pqxx::result res = txn.exec_params(
        "SELECT id FROM tasks WHERE author = $1 AND title = $2 LIMIT 1",
        author, title
    );

    if (res.empty())
        return -1;

    int id = res[0][0].as<int>();
    return id;    
}

const std::vector<NodeCLTask>& DBI::getAllFrom(const std::string& author) {
    nodes.clear();
    pqxx::work txn(*bridgeToDB);
    pqxx::result res = txn.exec_params(
        "SELECT author, title, content, created_at, completed FROM tasks WHERE author = $1 ORDER BY id",
        author
    );
    txn.commit();
    for (const auto& row : res) {
        NodeCLTask node;
        node.author = row["author"].c_str();
        node.title = row["title"].c_str();
        node.content = row["content"].c_str();
        node.created_at = row["created_at"].c_str();
        node.completed = row["completed"].c_str();
        nodes.push_back(std::move(node));
    }
    return nodes;
}