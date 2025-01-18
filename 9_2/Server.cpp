#include "Server.h"
#include <QDataStream>

Server::Server(int port, QObject *parent) : QObject(parent) {
    server = new QTcpServer(this);
    connect(server, &QTcpServer::newConnection, this, &Server::acceptConnection);

    if (!server->listen(QHostAddress::Any, port)) {
        throw std::runtime_error("Failed to start server: " + server->errorString().toStdString());
    }
}

void Server::acceptConnection() {
    QTcpSocket *clientSocket = server->nextPendingConnection();
    clients.push_back(clientSocket);
    connect(clientSocket, &QTcpSocket::readyRead, this, &Server::readClientData);
    connect(clientSocket, &QTcpSocket::disconnected, this, &Server::clientDisconnected);
}

void Server::readClientData() {
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (!socket) return;

    QByteArray data = socket->readAll();
    QDataStream stream(data);
    QPoint start, end;
    while (!stream.atEnd()) {
        stream >> start >> end;
        emit newLineDrawn(start, end);
        broadcastLine(start, end);
    }
}

void Server::broadcastLine(const QPoint &start, const QPoint &end) {
    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream << start << end;

    for (QTcpSocket *client : clients) {
        client->write(message);
    }
}

void Server::clientDisconnected() {
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    clients.erase(std::remove(clients.begin(), clients.end(), socket), clients.end());
    socket->deleteLater();
}
