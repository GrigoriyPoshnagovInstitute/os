#include "WhiteboardApp.h"
#include <QPainter>
#include <QMouseEvent>

WhiteboardApp::WhiteboardApp(int port, const QString &address, QWidget *parent)
        : QWidget(parent), isServer(address.isEmpty()), port(port), address(address) {
    setFixedSize(800, 600);
    setWindowTitle("Whiteboard");

    if (isServer) {
        server = new Server(port, this);
        connect(server, &Server::newLineDrawn, this, [this](const QPoint &start, const QPoint &end) {
            lines.emplace_back(start, end);
            update();
        });
    } else {
        client = new Client(address, port, this);
        connect(client, &Client::newLineReceived, this, [this](const QPoint &start, const QPoint &end) {
            lines.emplace_back(start, end);
            update();
        });
    }
}

void WhiteboardApp::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.fillRect(this->rect(), Qt::white);
    painter.setPen(QPen(Qt::black, 2));

    for (const auto &line : lines) {
        painter.drawLine(line.first, line.second);
    }
}

void WhiteboardApp::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        lastPos = event->pos();
    }
}

void WhiteboardApp::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        QPoint currentPos = event->pos();
        lines.emplace_back(lastPos, currentPos);

        sendLineToClients(lastPos, currentPos);

        lastPos = currentPos;
        update();
    }
}

void WhiteboardApp::sendLineToClients(const QPoint &start, const QPoint &end) {
    if (isServer && server) {
        server->broadcastLine(start, end);
    } else if (client) {
        client->sendLine(start, end);
    }
}
