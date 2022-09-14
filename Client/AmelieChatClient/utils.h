#ifndef UTILS_H
#define UTILS_H

#include <QWidget>

#include <QJsonObject>
#include <QSqlDatabase>

class Utils : public QWidget
{
    Q_OBJECT
public:
    ~Utils();
    static Utils &getInstance();
    QJsonObject &getJson();
    void saveJson();
    QString sha1sum(const QString &content);
    int registerToMySQL(const QString &username, const QString &password);
    int loginToMySQL(const QString &username, const QString &password);

    void toNetEndian(char *p, size_t size);
    void toHostEndian(char *p, size_t size);
signals:

public:

private:
    explicit Utils(QWidget *parent = nullptr);
    QJsonObject *json = nullptr;
    QSqlDatabase dbconn = QSqlDatabase::addDatabase("QMYSQL");
    bool isLittleEndian;
};

#endif // UTILS_H
