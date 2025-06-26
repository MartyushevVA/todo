#include "../include/server.hpp"

void DBI::add(const NodeDBTask& node) {
    try {
        pqxx::work txn(*bridgeToDB);
        txn.exec_params(
            "INSERT INTO tasks (author, title, content, completed) VALUES ($1, $2, $3, $4)",
            node.author, node.title, node.content, false
        );
        txn.commit();
        std::cout << "Task added successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Insert failed: " << e.what() << std::endl;
    }
}

void DBI::rm(const NodeDBTask& node) {
    try {
        pqxx::work txn(*bridgeToDB);
        txn.exec_params(
            "DELETE FROM tasks WHERE author = $1 AND title = $2",
            node.author, node.title
        );
        txn.commit();
        std::cout << "Task removed successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Removing failed: " << e.what() << std::endl;
    }
}

void DBI::chc(const NodeDBTask& node1, const NodeDBTask& node2) {
    try {
        pqxx::work txn(*bridgeToDB);
        txn.exec_params(
            "UPDATE tasks SET content = $1 WHERE author = $2 AND title = $3",
            node2.content, node1.author, node1.title
        );
        txn.commit();
        std::cout << "Task updated successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Update failed: " << e.what() << std::endl;
    }
}

void DBI::rear(const NodeDBTask& node1, const NodeDBTask& node2) {
    try {
        int id1 = getIdByTitle(node1.author, node1.title);
        int id2 = getIdByTitle(node1.author, node2.title);
        if (id1 == -1 || id2 == -1) {
            std::cerr << "Error: One of the records was not found." << std::endl;
            return;
        }

        pqxx::work txn(*bridgeToDB);

        txn.exec_params("UPDATE tasks SET id = -1 WHERE id = $1", id1);
        txn.exec_params("UPDATE tasks SET id = $1 WHERE id = $2", id1, id2);
        txn.exec_params("UPDATE tasks SET id = $1 WHERE id = -1", id2);

        txn.commit();
        std::cout << "IDs swapped successfully!" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Swap failed: " << e.what() << std::endl;
    }
}

void DBI::mad(const NodeDBTask& node) {
    try {
        pqxx::work txn(*bridgeToDB);
        txn.exec_params(
            "UPDATE tasks SET completed = true WHERE author = $1 AND title = $2",
            node.author, node.title
        );
        txn.commit();
        std::cout << "Task marked as done successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Marking failed: " << e.what() << std::endl;
    }
}

int DBI::getIdByTitle(const std::string& author, const std::string& title) {
    try {
        pqxx::work txn(*bridgeToDB);
        pqxx::result res = txn.exec_params(
            "SELECT id FROM tasks WHERE author = $1 AND title = $2 LIMIT 1",
            author, title
        );

        if (res.empty()) {
            std::cerr << "No record found for value: " << title << std::endl;
            return -1;
        }

        int id = res[0][0].as<int>();
        return id;

    } catch (const std::exception& e) {
        std::cerr << "Select failed: " << e.what() << std::endl;
        return -1;
    }
}

const std::vector<NodeCLTask>& DBI::getAllFrom(const std::string& author) {
    std::vector<NodeCLTask> nodes;
    try {
        pqxx::work txn(*bridgeToDB);
        pqxx::result res = txn.exec_params(
            "SELECT author, title, content, created_at, completed FROM tasks WHERE author = $1",
            author
        );

        for (const auto& row : res) {
            NodeCLTask node;
            node.author = row["author"].c_str();
            node.title = row["title"].c_str();
            node.content = row["content"].c_str();
            node.created_at = row["created_at"].c_str();
            node.completed = row["completed"].c_str();
            nodes.push_back(std::move(node));
        }

    } catch (const std::exception& e) {
        std::cerr << "Select failed: " << e.what() << std::endl;
    }

    return nodes;
}
