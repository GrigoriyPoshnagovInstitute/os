#ifndef WHITEBOARDAPP_H
#define WHITEBOARDAPP_H

#include <QWidget>
#include <QPoint>
#include <vector>
#include <QTcpSocket>
#include <QTcpServer>
#include "Server.h"
#include "Client.h"

class WhiteboardApp : public QWidget {
Q_OBJECT

public:
    explicit WhiteboardApp(int port, const QString &address = "", QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    bool isServer;
    int port;
    QString address;
    std::vector<std::pair<QPoint, QPoint>> lines;
    QPoint lastPos;

    Server *server = nullptr;
    Client *client = nullptr;

    void sendLineToClients(const QPoint &start, const QPoint &end);
};

#endif // WHITEBOARDAPP_H
