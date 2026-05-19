#include "controller/app_controller.h"
#include "common/constants.h"
#include <QCoreApplication>
#include <QJsonObject>
#include <QMessageBox>

AppController::AppController(QObject *parent) : QObject(parent) {
    setupConnections();
}

void AppController::start() {
    m_view.setUserInfo(m_configManager.getUserInfo());
    m_view.setAutoRunChecked(m_configManager.isAutoRunEnabled());
    m_view.show();

    m_authService.checkOnlineStatus();
    m_statusTimer.start(5000);

    checkAutoRunOnStart();
}

void AppController::raiseView() {
    m_view.showFromTray();
}

void AppController::setupConnections() {
    connect(&m_view, &MainWindow::loginRequested, this,
            &AppController::handleLogin);
    connect(&m_view, &MainWindow::logoutRequested, this,
            &AppController::handleLogout);
    connect(&m_view, &MainWindow::autoRunChanged, this,
            &AppController::handleAutoRunChanged);
    connect(&m_view, &MainWindow::quitRequested, qApp, &QCoreApplication::quit);

    connect(&m_authService, &AuthService::statusChanged, this,
            &AppController::handleStatusChanged);
    connect(&m_authService, &AuthService::loginFinished, this,
            &AppController::handleLoginFinished);
    connect(&m_authService, &AuthService::logoutFinished, this,
            [this](bool success, QString const &msg) {
                if (success) {
                    m_view.showMessage(Constants::StatusBarMsg::AlreadyOffline);
                    m_view.setOnlineStatus(false);
                }
            });
    connect(&m_authService, &AuthService::networkError, this,
            &AppController::handleNetworkError);

    connect(&m_statusTimer, &QTimer::timeout, &m_authService,
            &AuthService::checkOnlineStatus);
}

void AppController::handleLogin() {
    QJsonObject userInfo = m_view.getUserInfo();
    if (userInfo.value("rememberMe").toBool()) {
        m_configManager.saveUserInfo(userInfo);
    }

    m_view.showMessage(Constants::StatusBarMsg::WaitForProvider.arg(
        userInfo.value("provider").toString()));
    m_authService.login(userInfo.value("username").toString(),
                        userInfo.value("password").toString(),
                        userInfo.value("provider_data").toString(),
                        userInfo.value("mode_data").toInt());
}

void AppController::handleLogout() {
    m_authService.logout();
}

void AppController::handleAutoRunChanged(bool enabled) {
    m_configManager.setAutoRun(enabled);
}

void AppController::handleStatusChanged(bool online, QString const &msg) {
    m_isOnline = online;
    m_view.setOnlineStatus(online);
    m_view.showMessage(msg);
}

void AppController::handleLoginFinished(bool success, QString const &msg) {
    if (success) {
        m_view.setOnlineStatus(true);
        m_view.showMessage(Constants::StatusBarMsg::MsgFromProvider.arg(msg));
    } else {
        QMessageBox::warning(&m_view, "登录失败", msg);
    }
}

void AppController::handleNetworkError(QString const &errorMsg) {
    m_view.showMessage(Constants::StatusBarMsg::NetworkError.arg(errorMsg));
}

void AppController::checkAutoRunOnStart() {
    if (!QCoreApplication::arguments().contains("--auto-start")) {
        return;
    }

    if (!m_isOnline) {
        handleLogin();
    }

    QTimer::singleShot(1000, [this] {
        if (m_isOnline) {
            m_view.hideToBackground();
            m_view.showTrayMessage(
                APPNAME, Constants::TrayIconMsg::AutoHideToBackground);
        }
    });
}
