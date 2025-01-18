#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include <QObject>
#include <QPoint>

class Client : public QObject {
Q_OBJECT

public:
    Client(const QString &address, int port, QObject *parent = nullptr);
    void sendLine(const QPoint &start, const QPoint &end);

signals:
    void newLineReceived(const QPoint &start, const QPoint &end);

private slots:
    void readServerData();

private:
    QTcpSocket *socket;
};

#endif // CLIENT_H
