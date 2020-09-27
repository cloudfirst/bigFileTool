#include "mytool.h"
#include <QProcess>

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
#if defined(_WIN32)
    QString sipfile(QDir::toNativeSeparators(QDir::homePath()) + "\\oxfold\\bigfiletool\\myrouter\\webserver.txt");
#else
    QString sipfile(QDir::homePath() + "/oxfold/bigfiletool/myrouter/webserver.txt");
#endif
    QFile::remove(sipfile);
}

QString MyTool::getNodeIPV4()
{
    // const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    // for (const QHostAddress &address: QNetworkInterface::allAddresses())
    // {
    //     if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
    //          return address.toString();
    // }
#if defined(_WIN32)
    QString sipfile(QDir::toNativeSeparators(QDir::homePath()) + "\\oxfold\\bigfiletool\\myrouter\\webserver.txt");
#else
    QString sipfile(QDir::homePath() + "/oxfold/bigfiletool/myrouter/webserver.txt");
#endif
    QFile f(sipfile);
    if (!f.exists()) {
        return "0.0.0.0";
    } else {
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return "";
        } else {
            QByteArray ba = f.readAll();
            QString sip = QString::fromStdString(ba.toStdString());
            return sip;
        }
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
       rest = rest * 100 / mod;
       szResult += QString(".") + QString::number(rest).left(2);
    }
    szResult += units[i];
    return  szResult;
}

void MyTool::killAllProcess()
{
    QProcess p;
#if defined(_WIN32)
    p.start("taskkill /im oxfold-webtool.exe /f");
#else
    p.start("pkill oxfold-webtool");
#endif
    p.waitForFinished();
}

QString MyTool::getSharedDir()
{
#if defined(_WIN32)
    QString df(QDir::toNativeSeparators(QDir::homePath()) + "\\oxfold\\bigfiletool\\shared\\");
#else
    QString df(QDir::homePath() + "/oxfold/bigfiletool/shared/");
#endif
    return df;
}

QString MyTool::getDownloadedDir()
{
#if defined(_WIN32)
    QString df(QDir::toNativeSeparators(QDir::homePath()) + "\\oxfold\\bigfiletool\\downloaded\\");
#else
    QString df(QDir::homePath() + "/oxfold/bigfiletool/downloaded/");
#endif
    return df;
}

QString MyTool::getDownloadingDir()
{
#if defined(_WIN32)
    QString df(QDir::toNativeSeparators(QDir::homePath()) + "\\oxfold\\bigfiletool\\downloading\\");
#else
    QString df(QDir::homePath() + "/oxfold/bigfiletool/downloading/");
#endif
    return df;
}

QString MyTool::getMyrouterCredDir()
{
#if defined(_WIN32)
    QString df(QDir::toNativeSeparators(QDir::homePath()) + "\\oxfold\\bigfiletool\\myrouter\\");
#else
    QString df(QDir::homePath() + "/oxfold/bigfiletool/myrouter/");
#endif
    return df;
}

 QString MyTool::getClientCredDir()
 {
#if defined(_WIN32)
    QString df(QDir::toNativeSeparators(QDir::homePath()) + "\\oxfold\\bigfiletool\\client\\");
#else
    QString df(QDir::homePath() + "/oxfold/bigfiletool/client/");
#endif
    return df;
 }

 QString MyTool::getWebToolDir()
 {
#if defined(_WIN32)
    QString df(QDir::toNativeSeparators(QDir::homePath()) + "\\oxfold\\webtool\\");
#else
    QString df(QDir::homePath() + "/oxfold/webtool/");
#endif
    return df;
 }
