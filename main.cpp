#include "mainwindow.h"

#include <QApplication>
#include <QLocalSocket>
#include <QLocalServer>

bool isAlreadyRunning() {
    QLocalSocket socket;
    socket.connectToServer(APPNAME);

    // 已有实例存在
    if (socket.waitForConnected(100)) {
        socket.write("raise");
        socket.flush();
        socket.waitForBytesWritten(100);
        socket.disconnectFromServer();
        return true;
    }

    // 尝试清除之前未正常退出的 socket 文件
    QLocalServer::removeServer(APPNAME);
    return false;
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    if (isAlreadyRunning()) {
        return 0;
    }

    MainWindow w;
    w.show();

    // 建立本地 server
    QLocalServer server;
    if (!server.listen(APPNAME)) {
        return -1;
    }

    QObject::connect(&server, &QLocalServer::newConnection, [&]() {
        QLocalSocket *client = server.nextPendingConnection();
        if (!client) return;

        QObject::connect(client, &QLocalSocket::readyRead, [&]() {
            QByteArray msg = client->readAll();
            if (msg == "raise") {
                w.showFromTray();
            }
        });

        QObject::connect(client, &QLocalSocket::disconnected, client, &QLocalSocket::deleteLater);
    });

    return a.exec();
}
