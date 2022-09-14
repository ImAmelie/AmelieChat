#include "chatwindow.h"
#include "ui_chatwindow.h"

#include <QDebug>

ChatWindow::ChatWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatWindow)
{
    ui->setupUi(this);
}

ChatWindow::~ChatWindow()
{
    delete ui;
}

void ChatWindow::slotUpdatePicture(const QByteArray &data)
{
    QPixmap pic;
    pic.loadFromData(data, "JPG");
    ui->player->setPixmap(pic);
}
