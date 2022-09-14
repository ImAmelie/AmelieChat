#include "linkwindow.h"
#include "ui_linkwindow.h"

#include <QMessageBox>

extern "C" {
#include <hiredis/hiredis.h>
}

#include "mainwindow.h"

LinkWindow::LinkWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LinkWindow)
{
    ui->setupUi(this);
}

LinkWindow::~LinkWindow()
{
    delete ui;
}

void LinkWindow::on_cancelPushButton_clicked()
{
    this->close();
}

void LinkWindow::on_linkPushButton_clicked()
{
    QString username = ui->usernameLineEdit->text().trimmed();
    if (username == "") {
        QMessageBox::information(this, tr("用户名为空"), tr("用户名不能为空"));
        return;
    }

    QString peerAddr;
    quint16 peerPort;

    bool flag = false;

    // 向Resids服务器获得对方用户的IP和端口
    QJsonObject &json = utils.getJson();
    redisContext *context = redisConnect(json["RedisIP"].toString().toStdString().c_str(), json["RedisPort"].toString().toInt());
    if (context != nullptr) {
        freeReplyObject(redisCommand(context, "AUTH %s", json["RedisPassword"].toString().toStdString().c_str()));
        redisReply *reply = (redisReply *)redisCommand(context, "get AmelieChat_%s", username.toStdString().c_str());
        if (reply->type != REDIS_REPLY_STRING) {
            QMessageBox::information(this, tr("无法连接"), tr("该用户不存在或不在线！"));
        } else {
            std::string str(reply->str);
            peerAddr = QString(str.substr(0, str.find(':')).c_str());
            peerPort = std::stoul(str.substr(str.find(':') + 1));
            flag = true;
        }
        freeReplyObject(reply);
        redisFree(context);
    }

    if (flag) {
        dynamic_cast<MainWindow *>(this->parent())->signalConnectHost(peerAddr, peerPort);
        this->close();
    }
}

