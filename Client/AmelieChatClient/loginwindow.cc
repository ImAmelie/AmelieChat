#include "loginwindow.h"
#include "ui_loginwindow.h"

#include <QMessageBox>

#include "mainwindow.h"

LoginWindow::LoginWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::on_cancelPushButton_clicked()
{
    this->close();
}

void LoginWindow::on_loginPushButton_clicked()
{
    QString username = ui->usernameLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();

    if (username == "") {
        QMessageBox::information(this, tr("用户名为空"), tr("用户名不能为空！"));
        return;
    }
    if (password == "") {
        QMessageBox::information(this, tr("密码为空"), tr("密码不能为空！"));
        return;
    }

    int ret = utils.loginToMySQL(username, password);

    switch (ret) {
    case 0:
        QMessageBox::information(this, tr("恭喜"), tr("登录成功！"));
        emit dynamic_cast<MainWindow *>(this->parent())->loginSuccess(username);
        this->close();
        break;
    case 1:
        QMessageBox::information(this, tr("登陆失败"), tr("用户名或密码错误！"));
        break;
    case 2:
        QMessageBox::information(this, tr("连接参数错误"), tr("连接参数错误，请在主界面点击设置重新设定！"));
        break;
    }
}

