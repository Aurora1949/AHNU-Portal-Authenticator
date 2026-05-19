#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QJsonObject>
#include <QMainWindow>
#include <QMenu>
#include <QSystemTrayIcon>

QT_BEGIN_NAMESPACE

namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void setUserInfo(QJsonObject const &userInfo);
    QJsonObject getUserInfo() const;

    void setAutoRunChecked(bool checked);
    bool isAutoRunChecked() const;

    void setOnlineStatus(bool online);
    void showMessage(QString const &msg);
    void showTrayMessage(QString const &title, QString const &msg);

    void showFromTray();
    void hideToBackground();

signals:
    void loginRequested();
    void logoutRequested();
    void autoRunChanged(bool enabled);
    void quitRequested();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_loginBtn_clicked();
    void on_selfStartup_checkStateChanged();

private:
    void initUI();

    Ui::MainWindow *ui;
    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayMenu;
    bool m_isOnline = false;
};

#endif // MAINWINDOW_H
