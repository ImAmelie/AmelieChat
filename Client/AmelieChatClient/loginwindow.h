#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>

#include "utils.h"

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

private slots:
    void on_cancelPushButton_clicked();
    void on_loginPushButton_clicked();

private:
    Ui::LoginWindow *ui;
    Utils &utils = Utils::getInstance();
};

#endif // LOGINWINDOW_H
