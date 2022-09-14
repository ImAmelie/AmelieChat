#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QWidget>

#include <QTimer>
#include <QAudioInput>
#include <QAudioOutput>
#include <QBuffer>
#include <QMutex>

#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>

#include "utils.h"

class TcpControl;

namespace Ui {
class ChatWindow;
}

class ChatWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWindow(QWidget *parent = nullptr);
    ~ChatWindow();

public slots:
    void slotUpdatePicture(const QByteArray &data);

private:
    Ui::ChatWindow *ui;
    Utils &utils = Utils::getInstance();
};

#endif // CHATWINDOW_H
