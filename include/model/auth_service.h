#ifndef AUTH_SERVICE_H
#define AUTH_SERVICE_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

class AuthService : public QObject {
    Q_OBJECT
public:
    explicit AuthService(QObject *parent = nullptr);

    enum class LoginStatus {
        Failed,
        Success,
    };

    enum class RetCode {
        Unknown,
        WrongAccountOrPasswd,
        AlreadyOnline,
    };

    void login(QString const &username, QString const &password,
               QString const &provider, int mode);
    void logout();
    void checkOnlineStatus();

signals:
    void statusChanged(bool online, QString const &msg);
    void loginFinished(bool success, QString const &msg);
    void logoutFinished(bool success, QString const &msg);
    void networkError(QString const &errorMsg);

private:
    QNetworkAccessManager m_manager;
    bool m_isOnline = false;
};

#endif // AUTH_SERVICE_H
