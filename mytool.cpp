#include "mytool.h"
#include <QProcess>

MyTool::MyTool(QObject *parent) : QObject(parent)
{

}

void MyTool::init_bft_env()
{
    QDir dir1(MyTool::getSharedDir());
    if (!dir1.exists())
    {
        dir1.mkpath(MyTool::getSharedDir());
    }
    QDir dir2(MyTool::getDownloadedDir());
    if (!dir2.exists())
    {
        dir2.mkpath(MyTool::getDownloadedDir());
    }
    QDir dir3(MyTool::getDownloadingDir());
    if (!dir3.exists())
    {
        dir3.mkpath(MyTool::getDownloadingDir());
    }

    QDir dir4(MyTool::getMyrouterCredDir());
    if (!dir4.exists())
    {
        dir4.mkpath(MyTool::getMyrouterCredDir());
    }

    QDir dir5(MyTool::getClientCredDir());
    if (!dir5.exists())
    {
        dir5.mkpath(MyTool::getClientCredDir());
    }
}

void MyTool::close_bft_env()
{
    QFile::remove(MyTool::getMyrouterCredDir() + "webserver.txt");
}

QString MyTool::getNodeIPV4()
{
    QFile f(MyTool::getMyrouterCredDir() + "webserver.txt");
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
    QString df;
#if defined(_WIN32)
    df = (QDir::toNativeSeparators(QDir::homePath()) + "\\AppData\\oxfold\\bigfiletool\\shared\\");
#else
    df = (QDir::homePath() + "/oxfold/bigfiletool/shared/");
#endif
    return df;
}

QString MyTool::getDownloadedDir()
{
    QString df;
#if defined(_WIN32)
    df = (QDir::toNativeSeparators(QDir::homePath()) + "\\Downloads\\");
#else
    df = (QDir::homePath() + "/oxfold/bigfiletool/downloaded/");
#endif
    return df;
}

QString MyTool::getDownloadingDir()
{
    QString df;
#if defined(_WIN32)
    df = (QDir::toNativeSeparators(QDir::homePath()) + "\\Downloads\\");
#else
    df = (QDir::homePath() + "/oxfold/bigfiletool/downloading/");
#endif
    return df;
}

QString MyTool::getMyrouterCredDir()
{
    QString df;
#if defined(_WIN32)
   df = (QDir::toNativeSeparators(QDir::homePath()) + "\\AppData\\oxfold\\webtool\\myrouter\\");
#else
   #ifdef Q_OS_MACOS
       df = "/Library/Application Support/OxFold/BigFileTool/myrouter/";
   #else
       df =  "/usr/local/oxfold/bigfiletool/myrouter/");
   #endif
#endif
   return df;
}

 QString MyTool::getClientCredDir()
 {
     QString df;
#if defined(_WIN32)
    df = (QDir::toNativeSeparators(QDir::homePath()) + "\\AppData\\oxfold\\webtool\\client\\");
#else
    #ifdef Q_OS_MACOS
        df = "/Library/Application Support/OxFold/BigFileTool/client/";
    #else
        df =  "/usr/local/oxfold/bigfiletool/client/");
    #endif
#endif
    return df;
 }

 QString MyTool::getWebToolDir()
 {
     QString df;
#if defined(_WIN32)
    df = (QDir::toNativeSeparators(QDir::homePath()) + "\\AppData\\oxfold\\webtool\\");
#else
    #ifdef Q_OS_MACOS
        df = "/Library/Application Support/OxFold/BigFileTool/";
    #else
        df = "/usr/local/oxfold/bigfiletool/";
    #endif
#endif
    return df;
 }

 QString MyTool::getOxfoldWebTool()
 {
     QString df;

#if defined(_WIN32)
    if (MyTool::Win32or64() == "64") {
        df = "C:\\Program Files (x86)\\OxFold\\BigFileTool\\oxfold-webtool.exe";
    } else {
        df = "C:\\Program Files\\OxFold\\BigFileTool\\oxfold-webtool.exe";
    }
#else
    #ifdef Q_OS_MACOS
        df = "/Library/Application Support/OxFold/BigFileTool/oxfold-webtool";
    #else
        df = "/usr/bin/oxfold-webtool";
    #endif
#endif

    return df;
 }


 QString MyTool::Win32or64()
 {
    #ifdef Q_OS_WIN32
        bool h = QSysInfo::currentCpuArchitecture().contains(QLatin1String("64"));
        if (h) {
            return QString("64");
        } else {
            return QString("32");
        }
    #else
        return QString("64");
    #endif
 }
