#include "http_thread.h"
#include "httplib.h"
#include <QDir>
#include <QString>
#include <fstream>

using namespace httplib;
using namespace std;

////////////////////////////////////////////
/// HTTPThread_client::HTTPThread_client
///
///

HTTPThread_client::HTTPThread_client(QString dst_ip, int port, QString url)
{
    m_dst_ip = dst_ip;
    m_port   = port;
    m_url    = url;
}

void HTTPThread_client::run()
{
    httplib::Client cli(m_dst_ip.toStdString().c_str(), m_port);
    ofstream myfile;
    QString dst_file = QDir::homePath() + "/oxfold/bigfiletool/downloaded/" + m_url;

    myfile.open (dst_file.toStdString().c_str(), ios::out | ios::app | ios::binary);

    auto res = cli.Get(m_url.toStdString().c_str(),
       [&](const char *data, size_t data_length) {
           myfile.write(data, data_length);
           return true;
         },
        [&](uint64_t len, uint64_t total) {
          qDebug("%lld / %lld bytes => %d%% complete\n", len, total, (int)(len*100/total));
          return true; // return 'false' if you want to cancel the request.
        }
    );
}


////////////////////////////////////////////
///  HTTPThread_server::HTTPThread_server
///

HTTPThread_server::HTTPThread_server(QString ipstr, int port, QString root)
{
    m_ipstr = ipstr;
    m_port  = port;
    m_root  = root;
}


void HTTPThread_server::run()
{
    Server svr;

    svr.set_error_handler([](const Request & /*req*/, Response &res) {
        const char *fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
        char buf[BUFSIZ];
        snprintf(buf, sizeof(buf), fmt, res.status);
        res.set_content(buf, "text/html");
      });

   if (!svr.set_mount_point("/", m_root.toStdString().c_str())) {
       qDebug("The specified base directory doesn't exist...");
       return;
     }

   svr.listen(m_ipstr.toStdString().c_str(), m_port);
}
