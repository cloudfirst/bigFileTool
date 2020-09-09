#ifndef MYTOOL_H
#define MYTOOL_H

#include <QObject>
#include <QDir>
#include <QString>
#include <QNetworkInterface>
#include <QHostAddress>

class MyTool : public QObject
{
    Q_OBJECT
public:
    explicit MyTool(QObject *parent = nullptr);
    static void init_bft_env();
    static void close_bft_env();
    static QString getNodeIPV4();
    static QString converFileSizeToKBMBGB(qint64 filesize);

    static QString http_server_ip;

signals:

};

#endif // MYTOOL_H
