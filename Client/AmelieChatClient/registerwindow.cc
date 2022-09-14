#include "registerwindow.h"
#include "ui_registerwindow.h"

#include <QMessageBox>

RegisterWindow::RegisterWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterWindow)
{
    ui->setupUi(this);
}

RegisterWindow::~RegisterWindow()
{
    delete ui;
}

void RegisterWindow::on_cancelPushButton_clicked()
{
    this->close();
}

void RegisterWindow::on_registerPushButton_clicked()
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
    if (password != ui->password2LineEdit->text()) {
        QMessageBox::information(this, tr("密码不一致"), tr("两次输入的密码不一致！"));
        return;
    }

    int ret = utils.registerToMySQL(username, password);

    switch (ret) {
    case 0:
        QMessageBox::information(this, tr("恭喜"), tr("注册成功！"));
        this->close();
        break;
    case 1:
        QMessageBox::information(this, tr("注册失败"), tr("用户可能已存在，或其他错误！"));
        break;
    case 2:
        QMessageBox::information(this, tr("连接参数错误"), tr("连接参数错误，请在主界面点击设置重新设定！"));
        break;
    }
}

