#include "model/auth_service.h"
#include "common/constants.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkProxy>
#include <QUrlQuery>

AuthService::AuthService(QObject *parent) : QObject(parent) {
    m_manager.setProxy(QNetworkProxy::NoProxy);
}

void AuthService::login(QString const &username, QString const &password,
                        QString const &provider, int mode) {
    QUrl baseUrl(Constants::Network::BaseUrl);
    QUrlQuery query;
    QString account = (mode == 1) ? (username + "@" + provider) : username;
    query.addQueryItem("user_account", account);
    query.addQueryItem("user_password", password);
    baseUrl.setQuery(query);

    QNetworkRequest request(baseUrl);
    QNetworkReply *reply = m_manager.get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray d = reply->readAll();
            d.remove(0, 12); // remove jsonpReturn("
            d.chop(2);       // remove ")

            QJsonParseError err;
            QJsonDocument jd = QJsonDocument::fromJson(d, &err);
            if (err.error == QJsonParseError::NoError) {
                QJsonObject jo = jd.object();
                QString msg = jo.value("msg").toString();
                LoginStatus result =
                    static_cast<LoginStatus>(jo.value("result").toInt());
                RetCode code =
                    static_cast<RetCode>(jo.value("ret_code").toInt());

                if (result == LoginStatus::Success ||
                    (result == LoginStatus::Failed &&
                     code == RetCode::AlreadyOnline)) {
                    m_isOnline = true;
                    emit loginFinished(true, msg);
                    emit statusChanged(true, msg);
                } else {
                    emit loginFinished(false, msg);
                }
            } else {
                emit networkError("Data parse error: " + err.errorString());
            }
        } else {
            emit networkError(reply->errorString());
        }
        reply->deleteLater();
    });
}

void AuthService::logout() {
    QNetworkRequest request(QUrl(Constants::Network::LogoutUrl));
    QNetworkReply *reply = m_manager.get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray d = reply->readAll();
            d.remove(0, 12);
            d.chop(2);

            QJsonDocument jd = QJsonDocument::fromJson(d);
            QJsonObject jo = jd.object();
            QString msg = jo.value("msg").toString();
            m_isOnline = false;
            emit logoutFinished(true, msg);
            emit statusChanged(false, msg);
        } else {
            emit networkError(reply->errorString());
        }
        reply->deleteLater();
    });
}

void AuthService::checkOnlineStatus() {
    QNetworkRequest request(QUrl(Constants::Network::TestUrl));
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                         QNetworkRequest::ManualRedirectPolicy);
    QNetworkReply *reply = m_manager.head(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        if (reply->error() == QNetworkReply::NoError) {
            QUrl reUrl =
                reply->attribute(QNetworkRequest::RedirectionTargetAttribute)
                    .toUrl();
            bool online = (reUrl.host() != "rz.ahnu.edu.cn");
            if (online != m_isOnline) {
                m_isOnline = online;
                emit statusChanged(
                    online, online ? Constants::StatusBarMsg::AlreadyOnline
                                   : Constants::StatusBarMsg::AlreadyOffline);
            }
        }
        reply->deleteLater();
    });
}
