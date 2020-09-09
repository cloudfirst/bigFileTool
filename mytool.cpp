#include "mytool.h"

QString MyTool::http_server_ip = "0.0.0.0";

MyTool::MyTool(QObject *parent) : QObject(parent)
{

}

void MyTool::init_bft_env()
{
    //create directories at home dir
    // ~/oxfold/bigfiletool/shared
    // ~/oxfold/bigfiletool/downloaded
    // ~/oxfold/bigfiletool/downloading
    QString home = QDir::homePath();
    QDir dir1(QDir::homePath() + "/oxfold/bigfiletool/shared");
    if (!dir1.exists())
    {
        dir1.mkpath(QDir::homePath() + "/oxfold/bigfiletool/shared");
    }
    QDir dir2(QDir::homePath() + "/oxfold/bigfiletool/downloaded");
    if (!dir2.exists())
    {
        dir2.mkpath(QDir::homePath() + "/oxfold/bigfiletool/downloaded");
    }
    QDir dir3(QDir::homePath() + "/oxfold/bigfiletool/downloading");
    if (!dir3.exists())
    {
        dir3.mkpath(QDir::homePath() + "/oxfold/bigfiletool/downloading");
    }
    QDir dir4(QDir::homePath() + "/oxfold/bigfiletool/myrouter");
    if (!dir4.exists())
    {
        dir4.mkpath(QDir::homePath() + "/oxfold/bigfiletool/myrouter");
    }
}

void MyTool::close_bft_env()
{

}

QString MyTool::getNodeIPV4()
{
    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    for (const QHostAddress &address: QNetworkInterface::allAddresses())
    {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
             return address.toString();
    }
}

QString MyTool::converFileSizeToKBMBGB(qint64 filesize)
{
    QStringList units;
    units << "B" << "KB" << "MB" << "GB" << "TB" << "PB";
    double mod  = 1024.0;
    double size = filesize;
    //qDebug() << size;
    int i = 0;
    long rest = 0;
    while (size >= mod && i < units.count()-1 )
    {
        rest= (long)size % (long)mod ;
        size /= mod;
        i++;
    }
    QString szResult = QString::number(floor(size));
    if( rest > 0)
    {
       szResult += QString(".") + QString::number(rest).left(2);
    }
    szResult += units[i];
    return  szResult;
}
