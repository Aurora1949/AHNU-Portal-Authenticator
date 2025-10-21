#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QStandardPaths>
#include <QUrlQuery>
#include <QSettings>
#include <QCloseEvent>
#include <QTimer>
#include <QNetworkProxy>

#include "ui_mainwindow.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
      , ui(new Ui::MainWindow)
      , provider({{"中国移动", "cmcc"}, {"中国电信", "telecom"}, {"中国联通", "unicom"}})
      , baseUrl("http://100.64.4.10:801/eportal/portal/login")
      , testUrl("http://www.baidu.com")
      , logoutUrl("http://100.64.4.10:801/eportal/portal/logout")
      , isOnline(false)
      , trayMenu(new QMenu(this))
      , trayIcon(new QSystemTrayIcon(this))
      , networkChecker(new QTimer(this)) {
    ui->setupUi(this);
    init();
    getUserInfo();
    testOnline();
    doAutoRun();
}

void MainWindow::init() {
    setWindowTitle("AHNU上号器");
    setWindowIcon(QIcon(":/images/logo.png"));
    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint & ~Qt::WindowMinimizeButtonHint);
    setFixedSize(400, 178);

    manager.setProxy(QNetworkProxy::NoProxy);

    ui->statusbar->setSizeGripEnabled(false);
    ui->logoLabel->setPixmap(QPixmap(":/images/banner.png"));
    for (const auto &p: provider)
        ui->providerComboBox->addItem(p.name, p.value);
    ui->selfStartup->setChecked(isAutoRunEnabled());

    trayIcon->setIcon(QIcon(":/images/logo.png"));
    trayIcon->setToolTip(TrayIconMsg::Offline);

    QAction *showAction = new QAction("显示窗口", this);
    QAction *exitAction = new QAction("退出", this);
    trayMenu->addAction(showAction);
    trayMenu->addSeparator();
    trayMenu->addAction(exitAction);

    trayIcon->setContextMenu(trayMenu);

    connect(networkChecker, &QTimer::timeout, this, &MainWindow::testOnline);

    connect(showAction, &QAction::triggered, this, &MainWindow::onShowFromTray);
    connect(exitAction, &QAction::triggered, this, &MainWindow::onQuitFromTray);
    connect(trayIcon, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger) // 单击托盘图标
            onShowFromTray();
    });
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_loginBtn_clicked() {
    doLogin();
}

void MainWindow::online() {
    ui->loginBtn->setText("登出");
    ui->loginBtn->setStyleSheet("color: red;");
    trayIcon->setToolTip(TrayIconMsg::Online);

    setInputEnable(false);
    networkChecker->start(5000);

    isOnline = true;
}

void MainWindow::offline() {
    ui->loginBtn->setText("登录");
    ui->loginBtn->setStyleSheet("color: black;");
    trayIcon->setToolTip(TrayIconMsg::Offline);

    setInputEnable(true);
    if (!this->isVisible())
        onShowFromTray();

    networkChecker->stop();

    isOnline = false;
}

void MainWindow::setInputEnable(bool enable) {
    ui->passwdLe->setEnabled(enable);
    ui->usernameLe->setEnabled(enable);
    ui->providerComboBox->setEnabled(enable);
    ui->rememberMe->setEnabled(enable);
}

void MainWindow::saveUserInfo() {
    // 获取 APPDATA 路径
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(appDataPath);
    if (!dir.exists())
        dir.mkpath(appDataPath);

    QString filePath = appDataPath + "/userinfo.json";
    QFile file(filePath);

    // 创建 JSON 对象
    QJsonObject userInfo;
    userInfo["username"] = ui->usernameLe->text();
    userInfo["password"] = ui->passwdLe->text();
    userInfo["provider"] = ui->providerComboBox->currentText();

    // 写入文件
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QJsonDocument doc(userInfo);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    } else {
        QMessageBox::warning(this, "注意", "无法保存账户与密码");
    }
}

void MainWindow::getUserInfo() {
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString filePath = appDataPath + "/userinfo.json";

    QFile file(filePath);
    QJsonObject userInfo;

    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isObject())
            userInfo = doc.object();
        file.close();

        ui->usernameLe->setText(userInfo.take("username").toString());
        ui->passwdLe->setText(userInfo.take("password").toString());
        ui->providerComboBox->setEditText(userInfo.take("provider").toString());
        ui->rememberMe->setChecked(true);
    }
}

void MainWindow::testOnline() {
    QNetworkRequest request{testUrl};
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                         QNetworkRequest::ManualRedirectPolicy);
    QNetworkReply *reply{manager.head(request)};
    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        if (reply->error() == QNetworkReply::NoError) {
            QUrl reUrl{reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl()};
            if (reUrl.host() == "rz.ahnu.edu.cn") {
                ui->statusbar->showMessage(StatusBarMsg::AlreadyOffline);
                offline();
            } else {
                ui->statusbar->showMessage(StatusBarMsg::AlreadyOnline);
                online();
            }
        }
        reply->deleteLater();
    });
}

void MainWindow::setAutoRun(bool isStart) {
    QString appName = QApplication::applicationName(); // 获取应用名称
    QSettings settings(RegKey, QSettings::NativeFormat); // 创建QSettings对象

    if (isStart) {
        QString appPath = QApplication::applicationFilePath(); // 获取应用路径
        appPath += " --auto-start";
        settings.setValue(appName, appPath.replace("/", "\\")); // 写入注册表
    } else {
        settings.remove(appName); // 从注册表中删除
    }
}

bool MainWindow::isAutoRunEnabled() {
    QSettings settings(RegKey, QSettings::NativeFormat);
    QString appPath = QDir::toNativeSeparators(QApplication::applicationFilePath());
    return settings.value(QApplication::applicationName()).toString().split(" ")[0] == appPath;
}

void MainWindow::on_selfStartup_checkStateChanged() {
    setAutoRun(ui->selfStartup->isChecked());
}

void MainWindow::closeEvent(QCloseEvent *event) {
    hideToBackground();
    event->ignore(); // 阻止真正关闭
}

void MainWindow::onShowFromTray() {
    showNormal();
    activateWindow(); // 把窗口带到前台
    trayIcon->hide();
}

void MainWindow::onQuitFromTray() {
    qApp->quit();
}

void MainWindow::showFromTray() {
    onShowFromTray();
}

void MainWindow::doAutoRun() {
    if (!QCoreApplication::arguments().contains("--auto-start"))
        return;

    if (!isOnline) {
        doLogin();
    }

    QTimer::singleShot(500, [this] {
        if (isOnline) {
            hideToBackground();
            trayIcon->showMessage(APPNAME, TrayIconMsg::AutoHideToBackground);
        }
    });
}

void MainWindow::doLogin() {
    QNetworkRequest request;

    if (ui->rememberMe->isChecked()) {
        saveUserInfo();
    }

    if (isOnline) {
        request.setUrl(logoutUrl);
    } else {
        ui->statusbar->showMessage(QString(StatusBarMsg::WaitForProvider).arg(ui->providerComboBox->currentText()));

        QUrlQuery query;
        query.addQueryItem("user_account", ui->usernameLe->text());
        query.addQueryItem("user_password", ui->passwdLe->text());
        query.addQueryItem("provider", ui->providerComboBox->currentData().toString());
        baseUrl.setQuery(query);

        request.setUrl(baseUrl);
    }

    QNetworkReply *reply{manager.get(request)};

    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray d{reply->readAll()};
            d.remove(0, 12); // remove jsonpReturn("
            d.chop(2); // remove ")

            QJsonParseError err;
            QJsonDocument jd{QJsonDocument::fromJson(d, &err)};
            QString msg;

            if (err.error != QJsonParseError::NoError) {
                QMessageBox::critical(this, "数据解析错误", err.errorString() + "\n" + d);
                reply->deleteLater();
                ui->statusbar->clearMessage();
                return;
            }
            QJsonObject jo{jd.object()};

            if (isOnline) {
                if (jo.contains("msg"))
                    msg = jo.take("msg").toString();
                offline();
            } else {
                LoginStatus result{0};
                RetCode code{0};
                if (jo.contains("msg"))
                    msg = jo.take("msg").toString();

                if (jo.contains("result"))
                    result = static_cast<LoginStatus>(jo.take("result").toInt());

                if (jo.contains("ret_code") && jo.value("ret_code").isDouble()) {
                    code = static_cast<RetCode>(jo.take("ret_code").toInt());
                }

                if (result == LoginStatus::Success
                    || (result == LoginStatus::Failed && code == RetCode::AlreadyOnline)) {
                    online();
                }
            }

            ui->statusbar->showMessage(QString(StatusBarMsg::MsgFromProvider).arg(msg));
        } else {
            ui->statusbar->showMessage(QString(StatusBarMsg::NetworkError).arg(reply->errorString()));
        }

        reply->deleteLater();
    });
}

void MainWindow::hideToBackground() {
    hide();
    trayIcon->show();
}
