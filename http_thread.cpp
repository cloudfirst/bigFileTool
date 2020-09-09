#include "http_thread.h"
#include "civetweb_main.h"
#include <QDir>
#include <QString>


using namespace std;

////////////////////////////////////////////
/// HTTPThread_client::HTTPThread_client
///
///
///
HTTPThread_client::HTTPThread_client(QObject *parent) : QThread(parent)
{

}

HTTPThread_client::~HTTPThread_client()
{
    qDebug("HTTPThread_client terminated.");
}

void HTTPThread_client::init_thread(QString dst_ip, int port, QString url)
{
    m_dst_ip = dst_ip;
    m_port   = port;
    m_url    = url;
}

void HTTPThread_client::run()
{
    QString dst_file = QDir::homePath() + "/oxfold/bigfiletool/downloading/" + m_url + ".downloading";
    QByteArray text1 = dst_file.toUtf8();
    char *dr1 = new char[text1.size() + 1];
    strcpy(dr1, text1.data());

    QString url = "http://" + m_dst_ip + ":8080" +  "/" + m_url;
    QByteArray text2 = url.toUtf8();
    char *dr2 = new char[text2.size() + 1];
    strcpy(dr2, text2.data());

    qDebug("HTTPThread_client start to download file:%s", url.toStdString().c_str());
    run_client(dr2, dr1);
    qDebug("HTTPThread_client finish to download file:%s", url.toStdString().c_str());
    free(dr1);
    free(dr2);

    if (1) {
        // rename and move file to downloaded dir
        QString old_file = QDir::homePath() + "/oxfold/bigfiletool/downloading/" + m_url + ".downloading";
        QString new_file = QDir::homePath() + "/oxfold/bigfiletool/downloaded/" + m_url;
        QFile::rename(old_file, new_file);

        // remove downloading info
        QString old_info_file = QDir::homePath() + "/oxfold/bigfiletool/downloading/" + m_url + ".info";
        QFile(old_info_file).remove();
    }
}


////////////////////////////////////////////
///  HTTPThread_server::HTTPThread_server
///
HTTPThread_server::HTTPThread_server(QObject *parent) : QThread(parent)
{

}

HTTPThread_server::~HTTPThread_server()
{
    qDebug("HTTPThread_server terminated.");
}


void HTTPThread_server::init_thread(QString ipstr, int port, QString root)
{
    m_ipstr = ipstr;
    m_port  = port;
    m_root  = root;
}

void HTTPThread_server::run()
{
   char *configuration[4];
   configuration[0] = (char *)"-listening_ports";
   configuration[1] = (char *)"8080";
   configuration[2] = (char *)"-document_root";

   QByteArray text = m_root.toUtf8();
   char *dr = new char[text.size() + 1];
   strcpy(dr, text.data());
   configuration[3] = dr;

   qDebug("HTTPThread_server starte to listen on %s:%d... ", m_ipstr.toStdString().c_str(), m_port);
   civetweb_main(4, configuration);
   qDebug("HTTPThread_server stop listening on %s:%d... ", m_ipstr.toStdString().c_str(), m_port);
   free(dr);

}
