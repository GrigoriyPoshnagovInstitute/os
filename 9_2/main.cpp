#include <QApplication>
#include <iostream>
#include "WhiteboardApp.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    if (argc < 3) {
        std::cerr << "Usage: whiteboard <mode> <port> [address]" << std::endl;
        std::cerr << "Mode can be 'server' or 'client'" << std::endl;
        return -1;
    }

    std::string mode = argv[1];
    unsigned short port;
    try {
        port = std::stoi(argv[2]);
    } catch (const std::invalid_argument &e) {
        std::cerr << "Invalid port: " << argv[2] << std::endl;
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    QString address;
    if (mode == "client" && argc >= 4) {
        address = QString::fromStdString(argv[3]);
    } else if (mode == "client") {
        std::cerr << "Client mode requires address." << std::endl;
        return -1;
    }

    WhiteboardApp *appWindow = nullptr;
    if (mode == "server") {
        appWindow = new WhiteboardApp(port);
    } else if (mode == "client" && !address.isEmpty()) {
        appWindow = new WhiteboardApp(port, address);
    } else {
        std::cerr << "Invalid arguments!" << std::endl;
        return -1;
    }

    if (appWindow) {
        appWindow->show();
    }

    return app.exec();
}
