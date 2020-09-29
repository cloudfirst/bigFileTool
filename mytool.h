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
    static void killAllProcess();

    static QString getSharedDir();
    static QString getDownloadedDir();
    static QString getDownloadingDir();
    static QString getMyrouterCredDir();
    static QString getClientCredDir();
    static QString getWebToolDir();
    static QString getOxfoldWebTool();
    static QString Win32or64();
signals:

};

#endif // MYTOOL_H

/*
 * Mac OS X安装包的目录设计
 *
 * /Applications/BigFileTool/bigfiletool.app
 * /Library/Application Support/OxFold/BigFileTool/oxfold-webtool + zt_share.dylib
 * /Library/Application Support/OxFold/BigFileTool/myrouter/   chown -R luhya:staff myrouter
 * /Library/Application Support/OxFold/BigFileTool/client/     chown -R luhya:staff client
 * $HOME/oxfold/bigfiletool/shared/
 * $HOME/oxfold/bigfiletool/downloaded/
 * $HOME/oxfold/bigfiletool/downloading/
 *
 * Windows安装包的目录设计
 *
 * C:\Program Files(x86)\OxFold\BigFileTool\bigfiletool.exe
 * C:\Program Files(x86)\OxFold\BigFileTool\oxfold-webtool.exe
 * %HOME%\AppData\oxfold\bigfiletool\shared\
 * %HOME%\AppData\oxfold\bigfiletool\downloaded\
 * %HOME%\AppData\oxfold\bigfiletool\downloading\
 * %HOME%\AppData\oxfold\webtool\myrouter\
 * %HOME%\AppData\oxfold\webtool\client\
 *
 * Linux安装包的目录设计
 *
 * /usr/local/oxfold/bigfiletool/bigfiletool    -> /usr/bin/bigfiletool
 * /usr/local/oxfold/bigfiletool/oxfold-webtool -> /usr/bin/oxfold-webtool
 * /usr/local/oxfold/bigfiletool/myrouter/      chown $USER:$USER myrouter
 * /usr/local/oxfold/bigfiletool/client         chown $USER:$USER myrouter
 * $HOME/oxfold/bigfiletool/shared/
 * $HOME/oxfold/bigfiletool/downloaded/
 * $HOME/oxfold/bigfiletool/downloading/
 */
