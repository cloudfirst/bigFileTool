#include "mytool.h"
#include <QProcess>
#include <math.h>

QString MyTool::m_downlodas_dir = MyTool::init_downloads_dir();

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
#if defined(_WIN32)
    return MyTool::m_downlodas_dir + "\\";
#else
    return MyTool::m_downlodas_dir + "/";
#endif
}

QString MyTool::getDownloadingDir()
{
    return MyTool::getDownloadedDir();
}

QString MyTool::getMyrouterCredDir()
{
    QString df;
#if defined(_WIN32)
   df = (QDir::toNativeSeparators(QDir::homePath()) + "\\AppData\\oxfold\\webtool\\myrouter\\");
#else
   df =  "/usr/local/bigfiletool/myrouter/";
#endif
   return df;
}

 QString MyTool::getClientCredDir()
 {
     QString df;
#if defined(_WIN32)
    df = (QDir::toNativeSeparators(QDir::homePath()) + "\\AppData\\oxfold\\webtool\\client\\");
#else
    df =  "/usr/local/bigfiletool/client/";
#endif
    return df;
 }

 QString MyTool::getWebToolDir()
 {
     QString df;
#if defined(_WIN32)
    df = (QDir::toNativeSeparators(QDir::homePath()) + "\\AppData\\oxfold\\webtool\\");
#else
    df = "/usr/local/bigfiletool/";
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
    df = "/usr/local/bigfiletool/oxfold-webtool";
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

 void MyTool::set_downloads_dir(QString dir)
 {
     MyTool::m_downlodas_dir = dir;
     QString download_config_file = MyTool::getWebToolDir() + "download_path.txt";
     QFile f(download_config_file);
     f.open(QIODevice::WriteOnly);
     f.write(dir.toStdString().c_str());
     f.close();
 }

 QString MyTool::init_downloads_dir()
 {
     QString df;
     QString download_config_file = MyTool::getWebToolDir() + "download_path.txt";
     QFile f(download_config_file);
     if (f.exists()) {
         f.open(QIODevice::ReadOnly);
         df = f.readLine();
         f.close();
     } else {
#if defined(_WIN32)
    df = (QDir::toNativeSeparators(QDir::homePath()) + "\\Downloads\\");
#else
    df = (QDir::homePath() + "/Downloads/");
#endif
     }

     return df;
 }

#if defined(_WIN32)
BOOL MyTool::GetCPUUserRateEx(HANDLE hProccess, double &dCPUUserRate)
{
    static DWORD s_dwTickCountOld = 0;
    static LARGE_INTEGER s_lgProcessTimeOld = { 0 };
    static DWORD s_dwProcessorCoreNum = 0;
    if (!s_dwProcessorCoreNum)
    {
        SYSTEM_INFO sysInfo = { 0 };
        GetSystemInfo(&sysInfo);
        s_dwProcessorCoreNum = sysInfo.dwNumberOfProcessors;
    }
    double dbProcCpuPercent = 0;
    BOOL bRetCode = FALSE;
    FILETIME CreateTime, ExitTime, KernelTime, UserTime;
    LARGE_INTEGER lgKernelTime;
    LARGE_INTEGER lgUserTime;
    LARGE_INTEGER lgCurTime;
    bRetCode = GetProcessTimes(hProccess, &CreateTime, &ExitTime, &KernelTime, &UserTime);
    if (bRetCode)
    {
        lgKernelTime.HighPart = KernelTime.dwHighDateTime;
        lgKernelTime.LowPart = KernelTime.dwLowDateTime;
        lgUserTime.HighPart = UserTime.dwHighDateTime;
        lgUserTime.LowPart = UserTime.dwLowDateTime;
        lgCurTime.QuadPart = (lgKernelTime.QuadPart + lgUserTime.QuadPart);
        if (s_lgProcessTimeOld.QuadPart)
        {
            DWORD dwElepsedTime = GetTickCount() - s_dwTickCountOld;
            dbProcCpuPercent = (double)(((double)((lgCurTime.QuadPart - s_lgProcessTimeOld.QuadPart) * 100)) / dwElepsedTime) / 10000;
            dbProcCpuPercent = dbProcCpuPercent / s_dwProcessorCoreNum;
        }
        s_lgProcessTimeOld = lgCurTime;
        s_dwTickCountOld = GetTickCount();
    }
    dCPUUserRate = dbProcCpuPercent;
    return bRetCode;
}
#endif
