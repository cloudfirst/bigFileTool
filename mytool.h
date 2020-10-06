#ifndef MYTOOL_H
#define MYTOOL_H

#include <QObject>
#include <QDir>
#include <QString>
#include <QNetworkInterface>
#include <QHostAddress>

#if defined(_WIN32)
#include "Windows.h"
#endif

class MyTool : public QObject
{
    Q_OBJECT
public:
    static QString m_downlodas_dir;

public:
    explicit MyTool(QObject *parent = nullptr);
    static void init_bft_env();
    static void close_bft_env();
    static QString getNodeIPV4();
    static QString converFileSizeToKBMBGB(qint64 filesize);

    static QString http_server_ip;
    static void killAllProcess();

    static QString getSharedDir();
    static QString getDownloadedDir();
    static QString getDownloadingDir();
    static QString getMyrouterCredDir();
    static QString getClientCredDir();
    static QString getWebToolDir();
    static QString getOxfoldWebTool();
    static QString Win32or64();

    static QString init_downloads_dir();
    static void set_downloads_dir(QString dir);
#if defined(_WIN32)
    static BOOL GetCPUUserRateEx(HANDLE hProccess, double &dCPUUserRate);
#endif
signals:

};

#endif // MYTOOL_H

/*
 * Windows安装包的目录设计
 *
 * C:\Program Files(x86)\OxFold\BigFileTool\bigfiletool.exe
 * C:\Program Files(x86)\OxFold\BigFileTool\oxfold-webtool.exe
 * %HOME%\AppData\oxfold\bigfiletool\shared\
 * %HOME%\Downloads\
 * %HOME%\Downloads\
 * %HOME%\AppData\oxfold\webtool\myrouter\
 * %HOME%\AppData\oxfold\webtool\client\
 *
 * Mac OS X安装包的目录设计
 *
 * /Applications/BigFileTool/bigfiletool.app
 * /Library/Application Support/OxFold/BigFileTool/oxfold-webtool + zt_share.dylib
 * /Library/Application Support/OxFold/BigFileTool/myrouter/   chown -R luhya:staff myrouter --> /usr/local/bigfiletool/myrouter
 * /Library/Application Support/OxFold/BigFileTool/client/     chown -R luhya:staff client   --> /usr/local/bigfiletool/client
 * $HOME/oxfold/bigfiletool/shared/
 * $HOME/Downloads/
 * $HOME/Downloads/
 *
 *
 * Linux安装包的目录设计
 *
 * /usr/local/bigfiletool/bigfiletool    -> /usr/bin/bigfiletool
 * /usr/local/bigfiletool/oxfold-webtool -> /usr/bin/oxfold-webtool
 * /usr/local/bigfiletool/myrouter/      chown $USER:$USER myrouter
 * /usr/local/bigfiletool/client         chown $USER:$USER myrouter
 * $HOME/oxfold/bigfiletool/shared/
 * $HOME/oxfold/bigfiletool/downloaded/
 * $HOME/oxfold/bigfiletool/downloading/
 */
