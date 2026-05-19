#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <QJsonObject>
#include <QObject>
#include <QString>

class ConfigManager : public QObject {
    Q_OBJECT
public:
    explicit ConfigManager(QObject *parent = nullptr);

    void saveUserInfo(QJsonObject const &userInfo);
    QJsonObject getUserInfo() const;

    void setAutoRun(bool enabled);
    bool isAutoRunEnabled() const;

private:
    QString getUserInfoFilePath() const;
};

#endif // CONFIG_MANAGER_H
