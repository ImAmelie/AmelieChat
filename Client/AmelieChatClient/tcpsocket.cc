#include "tcpsocket.h"

#include <QApplication>
#include <QMessageBox>
#include <QNetworkInterface>
#include <QThread>

extern "C" {
#include <hiredis/hiredis.h>
}

#include "mainwindow.h"
#include "chatwindow.h"

namespace {
    unsigned char opt;
    uint64_t recvLen;
    uint64_t len;
    uint64_t rPos;
    uint64_t wPos;
    char recvData[1024 * 1024 * 20];
    char okData[1024 * 1024 * 20];
}

TcpSocket::TcpSocket(MainWindow *mainWin, QObject *parent)
    : QObject(parent)
    , mainWin(mainWin)
{
    username = mainWin->username;

    server = new QTcpServer(this);

    QJsonObject &json = utils.getJson();
    QString networkInterfaceName = json["NetworkInterface"].toString();

    for (const QNetworkInterface &interface : QNetworkInterface::allInterfaces()) {
        if (interface.flags().testFlag(QNetworkInterface::IsUp)) {
            if (interface.humanReadableName() == networkInterfaceName) {
                for (const QNetworkAddressEntry &entry : interface.addressEntries()) {
                    if (entry.ip().toString().contains('.')) {
                        server->listen(entry.ip());
                        listenAddr = entry.ip().toString();
                        break;
                    }
                }
                break;
            }

        }
    }

    listenPort = server->serverPort();

    // 向Resids服务器发送IP:Port
    //QJsonObject &json = utils.getJson();
    redisContext *context = redisConnect(json["RedisIP"].toString().toStdString().c_str(), json["RedisPort"].toString().toInt());
    if (context != nullptr) {
        freeReplyObject(redisCommand(context, "AUTH %s", json["RedisPassword"].toString().toStdString().c_str()));
        freeReplyObject(redisCommand(context, "set AmelieChat_%s %s", username.toStdString().c_str(), (listenAddr + ":" + QString::number(listenPort)).toStdString().c_str()));
        redisFree(context);
    }

    connect(server, &QTcpServer::newConnection, this, &TcpSocket::slotNewConnect);

    connect(mainWin, &MainWindow::signalConnectHost, this, &TcpSocket::slotConnectHost, Qt::QueuedConnection);

    connect(this, &TcpSocket::signalCollectStart, this, &TcpSocket::collectStart, Qt::QueuedConnection);

    ::opt = 0;
    ::recvLen = 0;
    ::len = 0;
    ::rPos = 0;
    ::wPos = 0;
}

TcpSocket::~TcpSocket()
{
    this->collectStop();
    if (socket != nullptr) {
        socket->close();
        delete socket;
    }
    server->close();
}

void TcpSocket::slotNewConnect()
{
    if (socket != nullptr) {
        return;
    }
    socket = server->nextPendingConnection();
    QHostAddress peerAddr(socket->peerAddress().toIPv4Address());
    emit mainWin->signalAsk(tr("有新的连接请求"), tr("来自 ") + peerAddr.toString() + tr(" 的连接请求，是否接受？"));
    int ret = mainWin->askRet;
    if (ret == QMessageBox::Yes) {
        emit mainWin->signalCreateChatWindow();
        chatWin = mainWin->chatWin;
        connect(socket, &QTcpSocket::disconnected, [&]{
            socket->deleteLater();
            socket = nullptr;
        });
        connect(socket, &QTcpSocket::readyRead, this, &TcpSocket::recv);
        connect(this, &TcpSocket::signalSend, this, &TcpSocket::send);
        connect(socket, &QTcpSocket::disconnected, chatWin, &ChatWindow::close);
        connect(chatWin, &ChatWindow::destroyed, this, &TcpSocket::closeSocket);
    } else {
        socket->close();
        socket->deleteLater();
        socket = nullptr;
    }
}

void TcpSocket::slotConnectHost(const QString addr, quint16 port)
{
    if (socket != nullptr) {
        return;
    }
    socket = new QTcpSocket(this);
    socket->connectToHost(addr, port);

    if (socket->waitForConnected()) {
        socket->waitForReadyRead(300000);
        emit mainWin->signalCreateChatWindow();
        chatWin = mainWin->chatWin;
        connect(socket, &QTcpSocket::disconnected, [&]{
            socket->deleteLater();
            socket = nullptr;
        });
        connect(socket, &QTcpSocket::readyRead, this, &TcpSocket::recv);
        connect(this, &TcpSocket::signalSend, this, &TcpSocket::send);
        connect(socket, &QTcpSocket::disconnected, chatWin, &ChatWindow::close);
        connect(chatWin, &ChatWindow::destroyed, this, &TcpSocket::closeSocket);
    } else {
        socket->close();
        socket->deleteLater();
        socket = nullptr;
    }
}

void writeData(const QByteArray &arr)
{
    if (::wPos + arr.size() > sizeof(::recvData)) {
        uint64_t aLen = sizeof(::recvData) - ::wPos;
        uint64_t bLen = arr.size() - aLen;
        memcpy(::recvData + ::wPos, arr.constData(), aLen);
        memcpy(::recvData, arr.constData() + aLen, bLen);
        ::wPos = bLen;
    } else {
        memcpy(::recvData + ::wPos, arr.constData(), arr.size());
        ::wPos += arr.size();
        ::wPos %= sizeof(::recvData);
    }
    ::recvLen += arr.size();
}

void readData(void *dest, uint64_t len)
{
    char *p = (char *)dest;
    if (::rPos + len > sizeof(::recvData)) {
        uint64_t aLen = sizeof(::recvData) - ::rPos;
        uint64_t bLen = len - aLen;
        memcpy(p, ::recvData + ::rPos, aLen);
        memcpy(p + aLen, ::recvData, bLen);
        ::rPos = bLen;
    } else {
        memcpy(p, ::recvData + ::rPos, len);
        ::rPos += len;
        ::rPos %= sizeof(::recvData);
    }
    ::recvLen -= len;
}

void TcpSocket::recv()
{
    if (socket == nullptr) {
        return;
    }

    if (dataMutex.tryLock()) {
        QByteArray arr = socket->readAll();
        if (::recvLen + arr.size() > sizeof(::recvData)) {
            emit mainWin->signalAsk(tr("错误"), tr("缓冲区溢出") + " " + QString::number(::recvLen) + " " + QString::number(arr.size()));
            qApp->exit(1);
        }
        writeData(arr);

        while (true) {
            if (::len == 0 && ::recvLen > sizeof(::len)) {
                readData(&::len, sizeof(::len));
                utils.toHostEndian((char *)&::len, sizeof(::len));
            }
            if (::len != 0 && ::opt == 0 && ::recvLen > sizeof(::opt)) {
                readData(&::opt, sizeof(::opt));
            }
            if (::len != 0 && ::opt != 0 && ::recvLen >= ::len) {
                readData(&::okData, ::len);
                QByteArray arr(::okData, ::len);
                switch (::opt) {
                case 1:
                    if (chatWin != nullptr) {
                        emit this->signalUpdatePicture(arr);
                    }
                    break;
                case 2:
                    if (chatWin != nullptr) {
                        emit this->signalPlaySound(arr);
                    }
                    break;
                default:
                    qDebug() << "opt is ERROR!";
                    break;
                }
                ::len = 0;
                ::opt = 0;
                continue;
            }
            break;
        }
        dataMutex.unlock();
    }
}

void TcpSocket::send(unsigned char opt, const QByteArray &data)
{
    if (socket == nullptr) {
        return;
    }
    if (sendMutex.tryLock()) {
        uint64_t len = data.size();
        utils.toNetEndian((char *)&len, sizeof(uint64_t));
        if (socket->write((char *)&len, sizeof(uint64_t)) != sizeof(uint64_t)) {
            emit mainWin->signalAsk(tr("数据发送出错"), tr("数据发送出错！"));
            qApp->exit(1);
        }
        if (socket->write((char *)&opt, 1) != 1) {
            emit mainWin->signalAsk(tr("数据发送出错"), tr("数据发送出错！"));
            qApp->exit(1);
        }
        if (socket->write(data.constData(), data.size()) != data.size()) {
            emit mainWin->signalAsk(tr("数据发送出错"), tr("数据发送出错！"));
            qApp->exit(1);
        }
        socket->flush();
        sendMutex.unlock();
    }
}

void TcpSocket::collectStart()
{
    QJsonObject &json = utils.getJson();

    cap = cv::VideoCapture(utils.getJson()["Camera"].toInt());

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &TcpSocket::slotGetPicture);
    timer->start(100);

    QAudioFormat format;
    format.setChannelCount(2);
    format.setSampleRate(16000);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    QString microphone_str = json["Microphone"].toString();

    QList<QAudioDeviceInfo> list = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    for (const QAudioDeviceInfo &info : list) {
        if (info.deviceName() == microphone_str) {
            if (!info.isFormatSupported(format)) {
                format = info.nearestFormat(format);
            }
            microphone = new QAudioInput(info, format, this);
            break;
        }
    }

    QString speaker_str = json["Speaker"].toString();

    list = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    for (const QAudioDeviceInfo &info : list) {
        if (info.deviceName() == speaker_str) {
            speaker = new QAudioOutput(info, format, this);
            break;
        }
    }

    pQIODeviceIn = microphone->start();
    pQIODeviceIn->open(QIODevice::ReadWrite);
    connect(pQIODeviceIn, &QIODevice::readyRead, this, &TcpSocket::slotReadyReadAudio);

    connect(this, &TcpSocket::signalUpdatePicture, chatWin, &ChatWindow::slotUpdatePicture, Qt::QueuedConnection);

    pQIODeviceOut = speaker->start();
    connect(this, &TcpSocket::signalPlaySound, this, &TcpSocket::slotPlaySound);
}

void TcpSocket::collectStop()
{
    timer->stop();
    microphone->stop();
    speaker->stop();
}

void TcpSocket::slotGetPicture()
{
    if (picMutex.tryLock()) {
        cap >> image;
        // 从Mat转换成QImage
        cv::Mat dest;
        cv::cvtColor(image, dest, cv::COLOR_BGR2RGB);
        QImage img((uchar *)dest.data, dest.cols, dest.rows, (int)dest.step, QImage::Format_RGB888);

        QByteArray arr;
        QBuffer buff(&arr);
        buff.open(QIODevice::WriteOnly);
        img.save(&buff, "JPG");
        buff.close();

        emit this->signalSend(1, arr);
        picMutex.unlock();
    }
}

void TcpSocket::slotReadyReadAudio()
{
    if (microphone == nullptr || pQIODeviceIn == nullptr) {
        qDebug() << "microphone is not availabel";
        return;
    }
    emit this->signalSend(2, pQIODeviceIn->readAll());
}

void TcpSocket::slotPlaySound(const QByteArray &data)
{
    if (speaker == nullptr || pQIODeviceOut == nullptr) {
        qDebug() << "speaker is not availabel";
        return;
    }

    pQIODeviceOut->write(data);
}

void TcpSocket::closeSocket()
{
    if (socket != nullptr) {
        socket->close();
        socket->deleteLater();
        socket = nullptr;
    }
    ::opt = 0;
    ::recvLen = 0;
    ::len = 0;
    ::rPos = 0;
    ::wPos = 0;
}
