#pragma once
#include <string>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <cstring>
#include <sstream>
#include <memory>
#include <cstdlib>
#include <QTcpServer>
#include <QTcpSocket>
#include <QObject>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

enum class option {
    ADD,
    REM,
    CHC,
    REAR,
    MAD,
    SHOW
};

enum class authstatus {PRESENT, NO, WRONG};

struct Session {
    bool isAuthorized;
    QString username;
    QByteArray buffer;
    std::string pendingSignUp;

    Session();
};

struct Node {
    explicit Node(const std::string& name) : author(name){};
    Node() = default;
    std::string author = "unknown";
    std::string title = "";
    std::string content = "";
    std::string created_at = "0";
    std::string completed = "false";
};

class DBI {
public:
    explicit DBI(const QString& conninfo) {
        db = QSqlDatabase::addDatabase("QPSQL");
        QStringList params = conninfo.split(' ');
        for (const QString& param : params) {
            if (param.startsWith("dbname="))
                db.setDatabaseName(param.section('=', 1));
            else if (param.startsWith("user="))
                db.setUserName(param.section('=', 1));
            else if (param.startsWith("password="))
                db.setPassword(param.section('=', 1));
            else if (param.startsWith("host="))
                db.setHostName(param.section('=', 1));
            else if (param.startsWith("port="))
                db.setPort(param.section('=', 1).toInt());
        }
        if (!db.open())
            throw std::runtime_error("Database connection error: " + db.lastError().text().toStdString());
    }

    authstatus check(const std::string&, const std::string&);
    void createUser(const std::string&, const std::string&);

    void add(const Node&);
    void rm(const Node&);
    void chc(const Node&, const Node&);
    void rear(const Node&, const Node&);
    void mad(const Node&);
    const std::vector<Node>& getAllFrom(const std::string&);

private:
    QSqlDatabase db;
    int getIdByTitle(const std::string& author, const std::string& title);
    std::vector<Node> nodes;
};

class Server : public QObject{
    Q_OBJECT
public:
    Server(int listen_port, const char* db_conninfo, QObject *parent = nullptr);
    virtual ~Server(){};

private slots:
    void onNewConnection();
    void onReadyRead(QTcpSocket* clientSocket);

private:
    QTcpServer* server;
    QMap<QTcpSocket*, Session> clientSessions;

    void processAuth(QTcpSocket* clientSOcket, Session &session, const QString &line);
    void processQuery(QTcpSocket* clientSOcket, Session &session, const QString &line);

    authstatus tryAuth(const std::string&);
    void newUser(const std::string&);
    std::string handleOption(const std::string&, const std::string&);
    static option getOptionFromString(const std::string&);
    static std::pair<std::string, std::string> getRegData(const std::string&);
    
    DBI dbconnection;
};
