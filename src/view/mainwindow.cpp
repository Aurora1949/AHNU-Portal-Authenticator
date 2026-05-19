#include "view/mainwindow.h"
#include "common/constants.h"
#include "ui_mainwindow.h"
#include <QAction>
#include <QCloseEvent>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      m_trayIcon(new QSystemTrayIcon(this)),
      m_trayMenu(new QMenu(this)) {
    ui->setupUi(this);
    initUI();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::initUI() {
    setWindowTitle("AHNU上号器");
    setWindowIcon(QIcon(":/images/logo.png"));
    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint &
                   ~Qt::WindowMinimizeButtonHint);
    setFixedSize(400, 200);

    ui->statusbar->setSizeGripEnabled(false);
    ui->logoLabel->setPixmap(QPixmap(":/images/banner.png"));

    struct Provider {
        QString name;
        QString value;
    };

    QList<Provider> providers = {
        {"中国移动", "cmcc"}, {"中国电信", "telecom"}, {"中国联通", "unicom"}};
    for (auto const &p: providers) {
        ui->providerComboBox->addItem(p.name, p.value);
    }

    ui->comboBoxMode->addItem("模式一", 1);
    ui->comboBoxMode->addItem("模式二", 2);

    m_trayIcon->setIcon(QIcon(":/images/logo.png"));
    m_trayIcon->setToolTip(Constants::TrayIconMsg::Offline);

    QAction *showAction = new QAction("显示窗口", this);
    QAction *exitAction = new QAction("退出", this);
    m_trayMenu->addAction(showAction);
    m_trayMenu->addSeparator();
    m_trayMenu->addAction(exitAction);
    m_trayIcon->setContextMenu(m_trayMenu);

    connect(showAction, &QAction::triggered, this, &MainWindow::showFromTray);
    connect(exitAction, &QAction::triggered, this, &MainWindow::quitRequested);
    connect(m_trayIcon, &QSystemTrayIcon::activated, this,
            [this](QSystemTrayIcon::ActivationReason reason) {
                if (reason == QSystemTrayIcon::Trigger) {
                    showFromTray();
                }
            });

    connect(ui->pushButtonQ, &QPushButton::clicked, this, [this] {
        QMessageBox::information(
            this, "模式说明",
            "模式一：需要购买校园卡，下行网速上限 "
            "100Mbps，需要选择正确的运营商。\n\n"
            "模式二：无需购买校园卡也可使用，下行网速上限 "
            "60Mbps，运营商选项在此模式下无效。\n\n"
            "如何选择？\n"
            "如果你有校园卡且不想受限于模式二的网速上，建议选择模式一并正确选择"
            "运营商；如果你没有校园卡或者不想购买，建议选择模式二。\n\n"
            "注意（仅高级用户）：\n"
            "模式一无法在校园内被其他设备访问，模式二则没有这个问题，如果你需要"
            "在校园内被其他设备访问（例如远程桌面），建议选择模式二。\n"
            "模式二无法使用git的ssh方式，如需使用，建议使用https "
            "git或选择模式一。");
    });
}

void MainWindow::setUserInfo(QJsonObject const &userInfo) {
    ui->usernameLe->setText(userInfo.value("username").toString());
    ui->passwdLe->setText(userInfo.value("password").toString());
    ui->providerComboBox->setEditText(userInfo.value("provider").toString());
    ui->comboBoxMode->setCurrentText(userInfo.value("mode").toString());
    ui->rememberMe->setChecked(true);
}

QJsonObject MainWindow::getUserInfo() const {
    QJsonObject userInfo;
    userInfo["username"] = ui->usernameLe->text();
    userInfo["password"] = ui->passwdLe->text();
    userInfo["provider"] = ui->providerComboBox->currentText();
    userInfo["provider_data"] = ui->providerComboBox->currentData().toString();
    userInfo["mode"] = ui->comboBoxMode->currentText();
    userInfo["mode_data"] = ui->comboBoxMode->currentData().toInt();
    userInfo["rememberMe"] = ui->rememberMe->isChecked();
    return userInfo;
}

void MainWindow::setAutoRunChecked(bool checked) {
    ui->selfStartup->setChecked(checked);
}

bool MainWindow::isAutoRunChecked() const {
    return ui->selfStartup->isChecked();
}

void MainWindow::setOnlineStatus(bool online) {
    m_isOnline = online;
    if (online) {
        ui->loginBtn->setText("登出");
        ui->loginBtn->setStyleSheet("color: red;");
        m_trayIcon->setToolTip(Constants::TrayIconMsg::Online);
        ui->passwdLe->setEnabled(false);
        ui->usernameLe->setEnabled(false);
        ui->providerComboBox->setEnabled(false);
        ui->rememberMe->setEnabled(false);
        ui->comboBoxMode->setEnabled(false);
    } else {
        ui->loginBtn->setText("登录");
        ui->loginBtn->setStyleSheet("color: black;");
        m_trayIcon->setToolTip(Constants::TrayIconMsg::Offline);
        ui->passwdLe->setEnabled(true);
        ui->usernameLe->setEnabled(true);
        ui->providerComboBox->setEnabled(true);
        ui->rememberMe->setEnabled(true);
        ui->comboBoxMode->setEnabled(true);
        if (!isVisible()) {
            showFromTray();
        }
    }
}

void MainWindow::showMessage(QString const &msg) {
    ui->statusbar->showMessage(msg);
}

void MainWindow::showTrayMessage(QString const &title, QString const &msg) {
    m_trayIcon->showMessage(title, msg);
}

void MainWindow::showFromTray() {
    showNormal();
    activateWindow();
    m_trayIcon->hide();
}

void MainWindow::hideToBackground() {
    hide();
    m_trayIcon->show();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    hideToBackground();
    event->ignore();
}

void MainWindow::on_loginBtn_clicked() {
    if (m_isOnline) {
        emit logoutRequested();
    } else {
        emit loginRequested();
    }
}

void MainWindow::on_selfStartup_checkStateChanged() {
    emit autoRunChanged(ui->selfStartup->isChecked());
}
