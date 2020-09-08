#include "http_thread.h"
#include "civetweb_main.h"
#include <QDir>
#include <QString>
#include <fstream>


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
//    httplib::Client cli(m_dst_ip.toStdString().c_str(), m_port);
//    ofstream myfile;
//    QString dst_file = QDir::homePath() + "/oxfold/bigfiletool/downloading/" + m_url + ".downloading";

//    myfile.open (dst_file.toStdString().c_str(), ios::out | ios::app | ios::binary);

//    QString url = "/" + m_url;
//    if (auto res = cli.Get(url.toStdString().c_str(),
//       [&](const char *data, size_t data_length) {
//           myfile.write(data, data_length);
//           return true;
//         },
//        [&](uint64_t len, uint64_t total) {
//          qDebug("http client thread: %lld / %lld bytes => %d%% complete\n", len, total, (int)(len*100/total));
//          emit update_thread_status(m_url, len, total);
//          return true; // return 'false' if you want to cancel the request.
//        }))
//    {
//        qDebug("status:%d", res->status);
//        //qDebug("content-type:%s", res->get_header_value("Content-Type"));
//    } else {
//        qDebug("error code: %d", res.error());
//    }
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
   configuration[1] = "8080";
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
