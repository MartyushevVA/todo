#include "server.hpp"

Server::Server(int listen_port, const char* db_conninfo, QObject *parent) : QObject(parent), dbconnection(db_conninfo) {
    server = new QTcpServer(this);
    connect(server, &QTcpServer::newConnection, this, &Server::onNewConnection);

    if (!server->listen(QHostAddress::Any, listen_port)) {
        qFatal("Failed to start server: %s", server->errorString().toUtf8().constData());
    } else{
        qDebug() << "Server listening on port" << listen_port;
    }
}

void Server::onNewConnection() {
    while (server->hasPendingConnections()) {
        QTcpSocket *clientSocket = server->nextPendingConnection();
        connect(clientSocket, &QTcpSocket::readyRead, this, [this, clientSocket]() { onReadyRead(clientSocket); });
        connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);

        qDebug() << "New client connected:" << clientSocket->peerAddress().toString() << ":" << clientSocket->peerPort();

        clientSocket->write("Enter [login passwd] please: ");
        clientSocket->flush();
        clientSessions[clientSocket] = Session();
    }
}

void Server::onReadyRead(QTcpSocket *clientSocket) {
    Session &session = clientSessions[clientSocket];
    QByteArray data = clientSocket->readAll();
    session.buffer += data;

    while (session.buffer.contains('\n')) {
        int idx = session.buffer.indexOf('\n');
        QString line = QString::fromUtf8(session.buffer.left(idx)).trimmed();
        session.buffer = session.buffer.mid(idx + 1);

        if (!session.isAuthorized) {
            processAuth(clientSocket, session, line);
        } else {
            processQuery(clientSocket, session, line);
        }
    }
}

void Server::processAuth(QTcpSocket *clientSocket, Session &session, const QString &line) {
    authstatus status = tryAuth(line.toStdString());

    if (status == authstatus::NO) {
        clientSocket->write("Confirm signing up: ");
        clientSocket->flush();
        session.pendingSignUp = line.toStdString();
    }
    else if (status == authstatus::PRESENT) {
        session.isAuthorized = true;
        session.username = QString::fromStdString(getRegData(line.toStdString()).first);
        clientSocket->write("Signed in successfully.\nEnter query: ");
        clientSocket->flush();
    }
    else {
        clientSocket->write("Wrong password. Try again: ");
        clientSocket->flush();
    }
}

void Server::processQuery(QTcpSocket *clientSocket, Session &session, const QString &line) {
    try {
        QString response = QString::fromStdString(handleOption(session.username.toStdString(), line.toStdString()));
        response += "\nEnter query: ";
        clientSocket->write(response.toUtf8());
        clientSocket->flush();
    } catch (...) {
        clientSocket->write("Wrong option. Try again: ");
        clientSocket->flush();
    }
}

Session::Session() : isAuthorized(false), pendingSignUp("") {}