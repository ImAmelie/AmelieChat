#ifndef REGISTERWINDOW_H
#define REGISTERWINDOW_H

#include <QDialog>

#include "utils.h"

namespace Ui {
class RegisterWindow;
}

class RegisterWindow : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterWindow(QWidget *parent = nullptr);
    ~RegisterWindow();

private slots:
    void on_cancelPushButton_clicked();
    void on_registerPushButton_clicked();

private:
    Ui::RegisterWindow *ui;
    Utils &utils = Utils::getInstance();
};

#endif // REGISTERWINDOW_H
