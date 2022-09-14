#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "utils.h"

class QThread;
class ChatWindow;
class TcpSocket;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void loginSuccess(const QString &username);
    void signalOpenChatWindow(const QString &peerAddr, quint16 peerPort);
    void signalConnectHost(const QString addr, quint16 port);
    void signalCreateChatWindow();
    void signalAsk(const QString &a, const QString &b);

public slots:
    void slotCreateChatWindow();
    void slotAsk(const QString &a, const QString &b);

private slots:
    void on_loginSuccess(const QString &username);

    void on_loginPushButton_clicked();
    void on_linkPushButton_clicked();
    void on_settingPushButton_clicked();
    void on_registerPushButton_clicked();
    void on_logoutPushButton_clicked();
    void on_exitPushButton_clicked();

public:
    QString username;
    ChatWindow *chatWin = nullptr;
    int askRet;

private:
    Ui::MainWindow *ui;
    Utils &utils = Utils::getInstance();
    bool isLogin = false;
    QThread *thread = nullptr;
    TcpSocket *tcpSocket = nullptr;
};
#endif // MAINWINDOW_H
