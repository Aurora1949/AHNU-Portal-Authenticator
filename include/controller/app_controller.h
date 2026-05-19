#ifndef APP_CONTROLLER_H
#define APP_CONTROLLER_H

#include "model/auth_service.h"
#include "model/config_manager.h"
#include "view/mainwindow.h"
#include <QObject>
#include <QTimer>

class AppController : public QObject {
    Q_OBJECT
public:
    explicit AppController(QObject *parent = nullptr);

    void start();
    void raiseView();

private slots:
    void handleLogin();
    void handleLogout();
    void handleAutoRunChanged(bool enabled);
    void handleStatusChanged(bool online, QString const &msg);
    void handleLoginFinished(bool success, QString const &msg);
    void handleNetworkError(QString const &errorMsg);

private:
    void setupConnections();
    void checkAutoRunOnStart();

    MainWindow m_view;
    AuthService m_authService;
    ConfigManager m_configManager;
    QTimer m_statusTimer;
    bool m_isOnline = false;
};

#endif // APP_CONTROLLER_H
