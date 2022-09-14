#include "utils.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTextStream>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QCoreApplication>
#include <QApplication>
#include <QCryptographicHash>
#include <QSqlQuery>
#include <QSqlError>

#include "def.h"

Utils::~Utils()
{
    if (json != nullptr) {
        delete json;
    }
}

Utils &Utils::getInstance()
{
    static Utils utils;
    return utils;
}

QJsonObject &Utils::getJson()
{
    if (json != nullptr) {
        return *json;
    }

    json = new QJsonObject;

    QFile file(CONFIGURE_PATH);
    QFileInfo info(file);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::critical(this, tr("配置文件打开失败"), tr("配置文件 ") + info.absoluteFilePath() + tr(" 打开失败，请检查是否存在该文件！"), QMessageBox::Ok, QMessageBox::NoButton);
        QCoreApplication::exit(1);
    }
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    QString str = stream.readAll();
    file.close();

    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(str.toUtf8(), &jsonError);
    if (jsonError.error != QJsonParseError::NoError) {
        QMessageBox::critical(this, tr("配置文件格式不正确"), tr("配置文件 ") + info.absoluteFilePath() + tr(" 格式不正确！"), QMessageBox::Ok, QMessageBox::NoButton);
        QCoreApplication::exit(1);
    }

    *json = doc.object();

    QStringList list = {
        "DBIP",
        "DBPort",
        "DBName",
        "DBUsername",
        "DBPassword",
        "RedisIP",
        "RedisPort",
        "RedisPassword",
        "Camera",
        "Microphone",
        "Speaker"
    };

    for (const QString &v : list) {
        if (!((*json).contains(v))) {
            QMessageBox::critical(this, tr("配置文件内容不正确"), tr("配置文件 ") + info.absoluteFilePath() + tr(" 内容不正确！"), QMessageBox::Ok, QMessageBox::NoButton);
            QCoreApplication::exit(1);
        }
    }

    return *json;
}

void Utils::saveJson()
{
    QFile file(CONFIGURE_PATH);
    QFileInfo info(file);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::information(this, tr("文件打开失败"), tr("配置文件 ") + info.absoluteFilePath() + tr(" 打开失败！"));
        return;
    }

    QJsonDocument doc(*json);

    file.write(doc.toJson());

    file.close();
}

Utils::Utils(QWidget *parent) : QWidget(parent)
{
    int i = 1;
    if (*(char *)&i == 1) {
        isLittleEndian = true;
    } else {
        isLittleEndian = false;
    }
}

QString Utils::sha1sum(const QString &content)
{
    QCryptographicHash hash(QCryptographicHash::Sha1);
    hash.addData(content.toUtf8());
    return hash.result().toHex();
}

/**
 * @brief Utils::registerToMySQL
 * @param username
 * @param password
 * @return 0 表示注册成功
 *         1 表示注册失败
 *         2 表示配置文件参数错误
 */
int Utils::registerToMySQL(const QString &username, const QString &password)
{
    if (json == nullptr) {
        this->getJson();
    }

    dbconn.setHostName(json->value("DBIP").toString());
    dbconn.setPort(json->value("DBPort").toInt());
    dbconn.setDatabaseName(json->value("DBName").toString());
    dbconn.setUserName(json->value("DBUsername").toString());
    dbconn.setPassword(json->value("DBPassword").toString());

    // TODO
    dbconn.setUserName("root");
    dbconn.setPassword("mysql");

    if (!dbconn.open()) {
        qDebug() << dbconn.lastError().text();
        dbconn.close();
        return 2;
    }

    QSqlQuery query(dbconn);
    query.prepare("INSERT INTO `user`(username, password) VALUES(:username,:password)");
    query.bindValue(":username", username);
    query.bindValue(":password", this->sha1sum(password));
    query.exec();

    int affectedRows = query.numRowsAffected();

    dbconn.close();

    if (affectedRows == 1) {
        return 0;
    } else {
        return 1;
    }
}

/**
 * @brief Utils::loginToMySQL
 * @param username
 * @param password
 * @return 0 表示登录成功
 *         1 表示登录失败
 *         2 表示配置文件参数错误
 */
int Utils::loginToMySQL(const QString &username, const QString &password)
{
    if (json == nullptr) {
        this->getJson();
    }

    dbconn.setHostName(json->value("DBIP").toString());
    dbconn.setPort(json->value("DBPort").toInt());
    dbconn.setDatabaseName(json->value("DBName").toString());
    dbconn.setUserName(json->value("DBUsername").toString());
    dbconn.setPassword(json->value("DBPassword").toString());

    // TODO
    dbconn.setUserName("root");
    dbconn.setPassword("mysql");

    if (!dbconn.open()) {
        qDebug() << dbconn.lastError().text();
        dbconn.close();
        return 2;
    }

    QSqlQuery query(dbconn);
    query.prepare("SELECT * FROM `user` WHERE `username`=:username and `password`=:password");
    query.bindValue(":username", username);
    query.bindValue(":password", this->sha1sum(password));
    query.exec();

    int size = query.size();

    dbconn.close();

    if (size == 1) {
        return 0;
    } else {
        return 1;
    }
}

void Utils::toNetEndian(char *p, size_t size)
{
    if (!isLittleEndian) {
        return;
    }
    char *l = p;
    char *r = p + size - 1;
    char tmp;
    while (l < r) {
        tmp = *l;
        *l = *r;
        *r = tmp;
        ++l;
        --r;
    }
}

void Utils::toHostEndian(char *p, size_t size)
{
    if (!isLittleEndian) {
        return;
    }
    char *l = p;
    char *r = p + size - 1;
    char tmp;
    while (l < r) {
        tmp = *l;
        *l = *r;
        *r = tmp;
        ++l;
        --r;
    }
}
