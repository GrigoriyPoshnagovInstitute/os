#include "Client.h"
#include <QDataStream>

Client::Client(const QString &address, int port, QObject *parent) : QObject(parent) {
    socket = new QTcpSocket(this);
    socket->connectToHost(address, port);

    if (!socket->waitForConnected(10000)) {
        throw std::runtime_error("Failed to connect to server: " + socket->errorString().toStdString());
    }

    connect(socket, &QTcpSocket::readyRead, this, &Client::readServerData);
}

void Client::sendLine(const QPoint &start, const QPoint &end) {
    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream << start << end;
    socket->write(message);
}

void Client::readServerData() {
    QByteArray data = socket->readAll();
    QDataStream stream(data);
    QPoint start, end;
    while (!stream.atEnd()) {
        stream >> start >> end;
        emit newLineReceived(start, end);
    }
}
