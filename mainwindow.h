#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <vector>
#include <QSystemTrayIcon>

QT_BEGIN_NAMESPACE

namespace Ui {
    class MainWindow;
}

QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

    void showFromTray();

private slots:
    void on_loginBtn_clicked();

    void on_selfStartup_checkStateChanged();

    void onShowFromTray();

    void onQuitFromTray();

private:
    void init();

    void online();

    void offline();

    void setInputEnable(bool);

    void saveUserInfo();

    void getUserInfo();

    void testOnline();

    void setAutoRun(bool);

    bool isAutoRunEnabled();

    void doAutoRun();

    void doLogin();

    void hideToBackground();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    struct Provider {
        QString name;
        QString value;
    };

    enum class LoginStatus {
        Failed,
        Success,
    };

    enum class RetCode {
        Unknown,
        WrongAccountOrPasswd,
        AlreadyOnline,
    };

    class TrayIconMsg {
    public:
        static constexpr char Online[]{"AHNU上号器正在后台运行\n目前状态：在线"};
        static constexpr char Offline[]{"AHNU上号器正在后台运行\n目前状态：离线"};
        static constexpr char AutoHideToBackground[]{"你已成功上线，AHNU上号器正在后台运行"};
    };

    class StatusBarMsg {
    public:
        static constexpr char WaitForProvider[]{"正在登录到%1..."};
        static constexpr char AlreadyOnline[]{"你已经连接上互联网"};
        static constexpr char NetworkError[]{"认证服务器连接错误：%1"};
        static constexpr char MsgFromProvider[]{"认证服务器：%1"};
        static constexpr char AlreadyOffline[]{"已断开连接"};
    };

    static constexpr char RegKey[]{"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"};

    Ui::MainWindow *ui;
    std::vector<Provider> provider;
    QUrl baseUrl;
    QUrl testUrl;
    QUrl logoutUrl;
    QNetworkAccessManager manager;
    bool isOnline;
    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
    QTimer *networkChecker;
};
#endif // MAINWINDOW_H
