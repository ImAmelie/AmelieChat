#include "settingswindow.h"
#include "ui_settingswindow.h"

#include <QAudio>
#include <QAudioDeviceInfo>
#include <QMessageBox>

#include <opencv2/videoio.hpp>

SettingsWindow::SettingsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);

    ui->DBIP->setText(json["DBIP"].toString());
    ui->DBPort->setText(json["DBPort"].toString());
    ui->DBName->setText(json["DBName"].toString());
    ui->DBUsername->setText(json["DBUsername"].toString());
    ui->DBPassword->setText(json["DBPassword"].toString());
    ui->RedisIP->setText(json["RedisIP"].toString());
    ui->RedisPort->setText(json["RedisPort"].toString());
    ui->RedisPassword->setText(json["RedisPassword"].toString());

    int cameraNum = 0;
    cv::VideoCapture camera;
    while (true) {
        if (!camera.open(cameraNum++)) {
            --cameraNum;
            break;;
        }
        camera.release();
    }
    camera.release();
    for (int i = 0; i != cameraNum; ++i) {
        ui->Camera->addItem(QString::number(i));
    }
    ui->Camera->setCurrentText(json["Camera"].toString());

    QList<QAudioDeviceInfo> microphones = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    for (const QAudioDeviceInfo &info : microphones) {
        ui->Microphone->addItem(info.deviceName());
    }
    ui->Microphone->setCurrentText(json["Microphone"].toString());

    QList<QAudioDeviceInfo> speakers = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    for (const QAudioDeviceInfo &info : speakers) {
        ui->Speaker->addItem(info.deviceName());
    }
    ui->Speaker->setCurrentText(json["Speaker"].toString());
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::on_cancelPushButton_clicked()
{
    this->close();
}

void SettingsWindow::on_savePushButton_clicked()
{
    if (ui->DBIP->text().trimmed() == "" ||
        ui->DBPort->text().trimmed() == "" ||
        ui->DBName->text().trimmed() == "" ||
        ui->DBUsername->text().trimmed() == "" ||
        ui->DBPassword->text().trimmed() == "" ||
        ui->RedisIP->text().trimmed() == "" ||
        ui->RedisPort->text().trimmed() == "" ||
        ui->RedisPassword->text().trimmed() == "") {
        QMessageBox::information(this, tr("输入不正确"), tr("输入内容不能为空！"));
        return;
    }

    json["DBIP"] = ui->DBIP->text().trimmed();
    json["DBPort"] = ui->DBPort->text().trimmed();
    json["DBName"] = ui->DBName->text().trimmed();
    json["DBUsername"] = ui->DBUsername->text().trimmed();
    json["DBPassword"] = ui->DBPassword->text().trimmed();
    json["RedisIP"] = ui->RedisIP->text().trimmed();
    json["RedisPort"] = ui->RedisPort->text().trimmed();
    json["RedisPassword"] = ui->RedisPassword->text().trimmed();

    json["Camera"] = ui->Camera->currentText();
    json["Microphone"] = ui->Microphone->currentText();
    json["Speaker"] = ui->Speaker->currentText();

    Utils::getInstance().saveJson();

    QMessageBox::information(this, tr("保存成功"), tr("保存成功！"));
}
