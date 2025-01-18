#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <vector>
#include <QObject>
#include <QPoint>

class Server : public QObject {
Q_OBJECT

public:
    explicit Server(int port, QObject *parent = nullptr);
    void broadcastLine(const QPoint &start, const QPoint &end);

signals:
    void newLineDrawn(const QPoint &start, const QPoint &end);

private slots:
    void acceptConnection();
    void readClientData();
    void clientDisconnected();

private:
    QTcpServer *server;
    std::vector<QTcpSocket *> clients;
};

#endif // SERVER_H
