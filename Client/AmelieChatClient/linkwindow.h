#ifndef LINKWINDOW_H
#define LINKWINDOW_H

#include <QDialog>

#include "utils.h"

namespace Ui {
class LinkWindow;
}

class LinkWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LinkWindow(QWidget *parent = nullptr);
    ~LinkWindow();

private slots:
    void on_cancelPushButton_clicked();
    void on_linkPushButton_clicked();

private:
    Ui::LinkWindow *ui;
    Utils &utils = Utils::getInstance();
};

#endif // LINKWINDOW_H
