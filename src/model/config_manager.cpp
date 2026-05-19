#include "model/config_manager.h"
#include "common/constants.h"
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QSettings>
#include <QStandardPaths>
#include <QTextStream>

ConfigManager::ConfigManager(QObject *parent) : QObject(parent) {}

QString ConfigManager::getUserInfoFilePath() const {
    QString appDataPath =
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(appDataPath);
    if (!dir.exists()) {
        dir.mkpath(appDataPath);
    }
    return appDataPath + "/" + Constants::Config::UserInfoFileName;
}

void ConfigManager::saveUserInfo(QJsonObject const &userInfo) {
    QFile file(getUserInfoFilePath());
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QJsonDocument doc(userInfo);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    }
}

QJsonObject ConfigManager::getUserInfo() const {
    QFile file(getUserInfoFilePath());
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isObject()) {
            return doc.object();
        }
    }
    return QJsonObject();
}

void ConfigManager::setAutoRun(bool enabled) {
    QString appName = QApplication::applicationName();
#ifdef Q_OS_WIN
    QSettings settings(Constants::Config::RegKey, QSettings::NativeFormat);
    if (enabled) {
        QString appPath = QApplication::applicationFilePath();
        appPath += " --auto-start";
        settings.setValue(appName, appPath.replace("/", "\\"));
    } else {
        settings.remove(appName);
    }
#elif defined(Q_OS_LINUX)
    QString autostartDir =
        QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) +
        "/autostart";
    QDir dir(autostartDir);
    if (!dir.exists()) {
        dir.mkpath(autostartDir);
    }

    QString desktopFilePath = autostartDir + "/" + appName + ".desktop";
    if (enabled) {
        QFile desktopFile(desktopFilePath);
        if (desktopFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            QTextStream out(&desktopFile);
            out << "[Desktop Entry]\n";
            out << "Type=Application\n";
            out << "Exec=" << QApplication::applicationFilePath()
                << " --auto-start\n";
            out << "Icon=ahnu\n";
            out << "Hidden=false\n";
            out << "NoDisplay=false\n";
            out << "X-GNOME-Autostart-enabled=true\n";
            out << "Name=" << appName << "\n";
            desktopFile.close();
        }
    } else {
        QFile::remove(desktopFilePath);
    }
#endif
}

bool ConfigManager::isAutoRunEnabled() const {
    QString appName = QApplication::applicationName();
#ifdef Q_OS_WIN
    QSettings settings(Constants::Config::RegKey, QSettings::NativeFormat);
    QString appPath =
        QDir::toNativeSeparators(QApplication::applicationFilePath());
    return settings.value(appName).toString().split(" ")[0] == appPath;
#elif defined(Q_OS_LINUX)
    QString desktopFilePath =
        QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) +
        "/autostart/" + appName + ".desktop";
    return QFile::exists(desktopFilePath);
#else
    return false;
#endif
}
