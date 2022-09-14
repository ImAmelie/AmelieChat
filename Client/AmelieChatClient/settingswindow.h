#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>

#include "utils.h"

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();

private slots:
    void on_cancelPushButton_clicked();
    void on_savePushButton_clicked();

private:
    Ui::SettingsWindow *ui;
    QJsonObject &json = Utils::getInstance().getJson();
};

#endif // SETTINGSWINDOW_H
