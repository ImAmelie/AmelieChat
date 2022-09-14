#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QThread>
#include <QMessageBox>

extern "C" {
#include <hiredis/hiredis.h>
}

#include "settingswindow.h"
#include "registerwindow.h"
#include "loginwindow.h"
#include "linkwindow.h"
#include "tcpsocket.h"
#include "chatwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(this, &MainWindow::loginSuccess, this, &MainWindow::on_loginSuccess);
    connect(this, &MainWindow::signalCreateChatWindow, this, &MainWindow::slotCreateChatWindow, Qt::BlockingQueuedConnection);
    connect(this, &MainWindow::signalAsk, this, &MainWindow::slotAsk, Qt::BlockingQueuedConnection);
}

MainWindow::~MainWindow()
{
    if (thread != nullptr) {
        thread->exit(0);
    }
    delete ui;
}

void MainWindow::on_loginSuccess(const QString &username)
{
    this->username = username;
    isLogin = true;
    ui->linkPushButton->setDisabled(false);

    thread = new QThread();
    tcpSocket = new TcpSocket(this);
    tcpSocket->moveToThread(thread);
    connect(thread, &QThread::finished, tcpSocket, &TcpSocket::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}

void MainWindow::on_loginPushButton_clicked()
{
    LoginWindow *win = new LoginWindow(this);
    win->setAttribute(Qt::WA_DeleteOnClose);
    win->exec();
}


void MainWindow::on_linkPushButton_clicked()
{
    LinkWindow *win = new LinkWindow(this);
    win->setAttribute(Qt::WA_DeleteOnClose);
    win->exec();
}


void MainWindow::on_settingPushButton_clicked()
{
    SettingsWindow *win = new SettingsWindow(this);
    win->setAttribute(Qt::WA_DeleteOnClose);
    win->exec();
}


void MainWindow::on_registerPushButton_clicked()
{
    RegisterWindow *win = new RegisterWindow(this);
    win->setAttribute(Qt::WA_DeleteOnClose);
    win->exec();
}


void MainWindow::on_logoutPushButton_clicked()
{
    if (thread != nullptr) {
        thread->exit(0);
        thread = nullptr;
    }
    isLogin = false;
    ui->linkPushButton->setDisabled(true);

    // 向Resids服务器发送del IP:Port
    QJsonObject &json = utils.getJson();
    redisContext *context = redisConnect(json["RedisIP"].toString().toStdString().c_str(), json["RedisPort"].toString().toInt());
    if (context != nullptr) {
        freeReplyObject(redisCommand(context, "AUTH %s", json["RedisPassword"].toString().toStdString().c_str()));
        freeReplyObject(redisCommand(context, "del AmelieChat_%s", username.toStdString().c_str()));
        redisFree(context);
    }
}


void MainWindow::on_exitPushButton_clicked()
{
    if (isLogin) {
        this->on_logoutPushButton_clicked();
    }
    qApp->exit(0);
}

void MainWindow::slotAsk(const QString &a, const QString &b)
{
    askRet = QMessageBox::information(nullptr, a, b, QMessageBox::Yes, QMessageBox::No);
}

void MainWindow::slotCreateChatWindow()
{
    if (chatWin != nullptr) {
        return;
    }
    chatWin = new ChatWindow(this);
    chatWin->setAttribute(Qt::WA_DeleteOnClose);
    chatWin->setWindowFlag(Qt::Window);
    chatWin->show();
    emit tcpSocket->signalCollectStart();
    connect(chatWin, &ChatWindow::destroyed, [&]{
        chatWin = nullptr;
    });
}
