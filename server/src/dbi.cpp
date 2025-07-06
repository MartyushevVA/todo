#include "../include/server.hpp"

authstatus DBI::check(const std::string& login, const std::string& passwd) {
    QSqlQuery query;
    query.prepare("SELECT passwd FROM users WHERE login = :login");
    query.bindValue(":login", QString::fromStdString(login));

    if (!query.exec()) {
        qWarning() << "DB error:" << query.lastError().text();
        return authstatus::WRONG;
    }

    if (!query.next())
        return authstatus::NO;

    QString db_passwd = query.value(0).toString();
    if (QString::fromStdString(passwd) == db_passwd)
        return authstatus::PRESENT;

    return authstatus::WRONG;
}

void DBI::createUser(const std::string& login, const std::string& passwd) {
    QSqlQuery query;
    query.prepare("INSERT INTO users (login, passwd) VALUES (:login, :passwd)");
    query.bindValue(":login", QString::fromStdString(login));
    query.bindValue(":passwd", QString::fromStdString(passwd));

    if (!query.exec()) {
        qWarning() << "DB error:" << query.lastError().text();
        throw std::runtime_error("Failed to create user");
    }
}

void DBI::add(const Node& node) {
    QSqlQuery query;
    query.prepare("INSERT INTO tasks (author, title, content, completed) VALUES (:author, :title, :content, :completed)");
    query.bindValue(":author", QString::fromStdString(node.author));
    query.bindValue(":title", QString::fromStdString(node.title));
    query.bindValue(":content", QString::fromStdString(node.content));
    query.bindValue(":completed", false);

    if (!query.exec() || query.numRowsAffected() == 0) {
        qWarning() << "DB error:" << query.lastError().text();
        throw std::runtime_error("Failed to add task");
    }
}

void DBI::rm(const Node& node) {
    QSqlQuery query;
    query.prepare("DELETE FROM tasks WHERE author = :author AND title = :title");
    query.bindValue(":author", QString::fromStdString(node.author));
    query.bindValue(":title", QString::fromStdString(node.title));

    if (!query.exec() || query.numRowsAffected() == 0) {
        qWarning() << "DB error:" << query.lastError().text();
        throw std::runtime_error("Failed to remove task");
    }
}

void DBI::chc(const Node& node1, const Node& node2) {
    QSqlQuery query;
    query.prepare("UPDATE tasks SET content = :content WHERE author = :author AND title = :title");
    query.bindValue(":content", QString::fromStdString(node2.content));
    query.bindValue(":author", QString::fromStdString(node1.author));
    query.bindValue(":title", QString::fromStdString(node1.title));

    if (!query.exec() || query.numRowsAffected() == 0) {
        qWarning() << "DB error:" << query.lastError().text();
        throw std::runtime_error("Failed to change task content");
    }
}

void DBI::rear(const Node& node1, const Node& node2) {
    int id1 = getIdByTitle(node1.author, node1.title);
    int id2 = getIdByTitle(node1.author, node2.title);

    if (id1 == -1 || id2 == -1)
        return;

    QSqlDatabase::database().transaction();

    QSqlQuery query;
    query.prepare("UPDATE tasks SET id = -1 WHERE id = :id1");
    query.bindValue(":id1", id1);
    if (!query.exec()) goto rollback;

    query.prepare("UPDATE tasks SET id = :id1 WHERE id = :id2");
    query.bindValue(":id1", id1);
    query.bindValue(":id2", id2);
    if (!query.exec()) goto rollback;

    query.prepare("UPDATE tasks SET id = :id2 WHERE id = -1");
    query.bindValue(":id2", id2);
    if (!query.exec() || query.numRowsAffected() == 0) goto rollback;

    QSqlDatabase::database().commit();
    return;

rollback:
    QSqlDatabase::database().rollback();
    qWarning() << "DB error in rear:" << query.lastError().text();
    throw std::runtime_error("Failed to reorder tasks");
}

void DBI::mad(const Node& node) {
    QSqlQuery query;
    query.prepare("UPDATE tasks SET completed = true WHERE author = :author AND title = :title");
    query.bindValue(":author", QString::fromStdString(node.author));
    query.bindValue(":title", QString::fromStdString(node.title));

    if (!query.exec() || query.numRowsAffected() == 0) {
        qWarning() << "DB error:" << query.lastError().text();
        throw std::runtime_error("Failed to mark task as done");
    }
}

int DBI::getIdByTitle(const std::string& author, const std::string& title) {
    QSqlQuery query;
    query.prepare("SELECT id FROM tasks WHERE author = :author AND title = :title LIMIT 1");
    query.bindValue(":author", QString::fromStdString(author));
    query.bindValue(":title", QString::fromStdString(title));

    if (!query.exec()) {
        qWarning() << "DB error:" << query.lastError().text();
        return -1;
    }

    if (query.next())
        return query.value(0).toInt();

    return -1;
}

const std::vector<Node>& DBI::getAllFrom(const std::string& author) {
    nodes.clear();

    QSqlQuery query;
    query.prepare("SELECT author, title, content, created_at, completed FROM tasks WHERE author = :author ORDER BY id");
    query.bindValue(":author", QString::fromStdString(author));

    if (!query.exec()) {
        qWarning() << "DB error:" << query.lastError().text();
        return nodes;
    }

    while (query.next()) {
        Node node;
        node.author = query.value("author").toString().toStdString();
        node.title = query.value("title").toString().toStdString();
        node.content = query.value("content").toString().toStdString();
        node.created_at = query.value("created_at").toString().toStdString();
        node.completed = query.value("completed").toString().toStdString();
        nodes.push_back(std::move(node));
    }

    return nodes;
}
