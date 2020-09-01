#ifndef HTTPTHREAD_H
#define HTTPTHREAD_H

#include <QThread>

class HTTPThread_client : public QThread
{
    Q_OBJECT

public:
    HTTPThread_client(QString dst_ip, int port, QString url);

protected:
    void run() override;

private:
    QString m_dst_ip;
    int     m_port;
    QString m_url;
};


class HTTPThread_server : public QThread
{
    Q_OBJECT

public:
    HTTPThread_server(QString ipstr, int port, QString root);

protected:
    void run() override;

private:
    QString m_ipstr;
    int     m_port;
    QString m_root;


};

#endif // HTTPTHREAD_H
