#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QMutex>
#include <QAudioInput>
#include <QAudioOutput>
#include <QBuffer>

#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>

#include "utils.h"

class MainWindow;
class ChatWindow;

class TcpSocket : public QObject
{
    Q_OBJECT
public:
    explicit TcpSocket(MainWindow *mainWin, QObject *parent = nullptr);
    ~TcpSocket();

signals:
    void signalCreateChatWindow();
    void signalUpdatePicture(const QByteArray &data);
    void signalPlaySound(const QByteArray &data);
    void signalSend(unsigned char opt, const QByteArray &data);
    void signalCollectStart();

public slots:
    void slotNewConnect();
    void slotConnectHost(const QString addr, quint16 port);
    void recv();
    void send(unsigned char opt, const QByteArray &data);
    void collectStart();
    void collectStop();
    void slotGetPicture();
    void slotReadyReadAudio();
    void slotPlaySound(const QByteArray &data);
    void closeSocket();

private:
    QTcpServer *server = nullptr;
    QTcpSocket *socket = nullptr;
    QString listenAddr;
    quint16 listenPort;
    QString username;

    QMutex sendMutex;
    QMutex dataMutex;
    Utils &utils = Utils::getInstance();
    MainWindow *mainWin = nullptr;
    ChatWindow *chatWin = nullptr;

    QMutex picMutex;
    // 摄像头
    cv::VideoCapture cap;
    cv::Mat image;
    QTimer *timer;
    // 麦克风
    QAudioInput *microphone = nullptr;
    QIODevice *pQIODeviceIn = nullptr;
    // 扬声器
    QAudioOutput *speaker = nullptr;
    QIODevice *pQIODeviceOut = nullptr;
    QBuffer soundBuff;
};

#endif // TCPSOCKET_H
