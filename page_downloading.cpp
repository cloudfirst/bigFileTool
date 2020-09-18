#include "page_downloading.h"
#include "ui_page_downloading.h"
#include <QResizeEvent>
#include <QFileDialog>
#include <QDateTime>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include "http_thread.h"
#include <QJsonArray>
#include <QProgressBar>
#include <QString>

#include "mytool.h"
#include "add_link_dialog.h"

#if defined(_WIN32)
#include "Windows.h"
#endif

Page_downloading::Page_downloading(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Page_downloading)
{
    ui->setupUi(this);
    init_table();
    this->m_Timer = NULL;

}

Page_downloading::~Page_downloading()
{
    delete ui;
    m_Timer->stop();
    delete  m_Timer;
    for (int i=0; i<this->http_client_array.size(); ++i)
    {
        http_client_array.at(i)->downloading_process->kill();
        delete  http_client_array.at(i);
    }
}

void Page_downloading::start_download_status_timer()
{
    if (m_Timer == NULL) {
        m_Timer = new QTimer(this);
        connect(m_Timer, SIGNAL(timeout()),this, SLOT(MyTimerSlot()));
    }
    m_Timer->start(1000);
}

QString convertSencond2HHMMSS(uint64_t time)
{
    int hour = time/3600;
    time = time%3600;
    int min = time/60;
    time = time%60;
    int sec = time;

    return QString::number(hour) + ":" + QString::number(min) + ":" + QString::number(sec);
}

void Page_downloading::MyTimerSlot()
{
    QTableWidget * t = ui->tableWidget;
    QProgressBar *mypgb;
    QTableWidgetItem *item;

    for (int i=0; i<this->http_client_array.size(); ++i)
    {
        Downloading_Task *dt = http_client_array.at(i);
        QString fname = dt->downloading_file_name;
        // get current file size
        uint64_t new_size = dt->size_in_5s.last() + 10 * (qrand() % ((9 + 1) - 1) + 1);
        dt->size_in_5s.pop_front();
        dt->size_in_5s.append(new_size);

        int percentage = 0;
        double bps = 0.0;
        uint64_t letf_seconds = 0;
        if (new_size >= dt->total_len) {
            percentage = 100;
        } else {
            percentage = new_size * 100 / dt->total_len;
            bps = (new_size - dt->size_in_5s.first()) / 5.0;
            letf_seconds = (dt->total_len - new_size) / bps;
        }

        // update tableWidget
        mypgb = (QProgressBar*)t->cellWidget(i, 3); // update progress bar
        mypgb->setValue(percentage);

        item = t->item(dt->row_in_tableWidge, 4);  // update speed
        QString _txt = MyTool::converFileSizeToKBMBGB((qint64)bps) + "/s" ;
        item->setText(_txt);

        item = t->item(dt->row_in_tableWidge, 5);  // update time left
        item->setText("剩余 " +  convertSencond2HHMMSS(letf_seconds));
    }
}

uint64_t getCurrentFileSize(QString fname)
{
#if defined(_WIN32)
    QString df(QDir::toNativeSeparators(QDir::homePath()) + "\\oxfold\\bigfiletool\\downloading\\" + fname + ".downloading");
#else
    QString df(QDir::homePath() + "/oxfold/bigfiletool/downloading/" + fname + ".downloading");
#endif
    QFileInfo  info(df);
    return info.size();
}

void Page_downloading::init_table()
{
    QTableWidget * t = ui->tableWidget;

    t->verticalHeader()->setVisible(false);
    t->setSelectionBehavior(QAbstractItemView::SelectRows);
    t->setSelectionMode( QAbstractItemView::SingleSelection );

    // init data
    int n_cols = 6;
#if defined(_WIN32)
     QDir dir1(QDir::toNativeSeparators(QDir::homePath()) + "\\oxfold\\bigfiletool\\downloading\\");
#else
    QDir dir1(QDir::homePath() + "/oxfold/bigfiletool/downloading/");
#endif
    QStringList filters;
    filters << "*.info";
    QFileInfoList list = dir1.entryInfoList(filters);
    QJsonArray downlaod_array;
    for (int i = 0; i < list.size(); ++i)
    {
        QFileInfo fileInfo = list.at(i);
        if (fileInfo.isDir())
            continue;

        QFile fp(fileInfo.absoluteFilePath());
        if (!fp.open(QIODevice::ReadOnly | QIODevice::Text))
            continue;
        QString fcontent = fp.readAll();
        fp.close();

        QJsonDocument doc = QJsonDocument::fromJson(fcontent.toUtf8());
        QJsonObject obj;
        if (!doc.isNull() && doc.isObject())
        {
            obj = doc.object();
            downlaod_array.append(obj);
        }
    }


    for (int index =0; index < downlaod_array.size();  index++)
    {
        QJsonObject obj = downlaod_array[index].toObject();
        t->insertRow(t->rowCount());
        // int rh = t->rowHeight(t->rowCount() - 1);
        // t->setRowHeight(t->rowCount() - 1, 50);

        uint64_t  c_size = getCurrentFileSize(obj["file_name"].toString());

        for (int col=0; col!=n_cols; ++col)
        {
            QTableWidgetItem * const i = new QTableWidgetItem;
            i->setFlags(i->flags() & ~Qt::ItemIsEditable);
            if (col == 0 ) { //选择
                //Checkbox
                i->setFlags(
                    Qt::ItemIsSelectable
                  | Qt::ItemIsUserCheckable
                );
                i->setCheckState(Qt::Unchecked);
                t->setItem(t->rowCount()-1, col, i);
            }
            if (col == 1 ) { //文件名
                i->setText(obj["file_name"].toString());
                t->setItem(t->rowCount()-1, col, i);
            }
            if (col == 2 ) {  //文件大小
                i->setText(MyTool::converFileSizeToKBMBGB(obj["file_size"].toDouble()));
                t->setItem(t->rowCount()-1, col, i);
            }
            if (col == 3 ) {  //progress bar
                //i->setText("stopped");
                QProgressBar *pgbar = new QProgressBar();
                pgbar->setRange(0,100);

                int percentage = c_size * 100 / obj["file_size"].toDouble();
                pgbar->setValue(percentage);

                t->setCellWidget(t->rowCount()-1, col, pgbar);
            }
            if (col == 4 ) { //下载速度
                i->setText("已暂停");
                t->setItem(t->rowCount()-1, col, i);
            }
            if (col == 5 ) { //剩余时间
                i->setText("0:0:0");
                t->setItem(t->rowCount()-1, col, i);
            }

        }

        QString url = "http://" + obj["host_ip"].toString() + ":8080" + "/" +obj["file_name"].toString();
#if defined(_WIN32)
        QString dst_file_name = QDir::toNativeSeparators(QDir::homePath()) + "\\oxfold\\bigfiletool\\downloading\\" + obj["file_name"].toString() + ".downloading";
        QString client_cred_path = QDir::toNativeSeparators(QDir::homePath()) + "\\oxfold\\bigfiletool\\client\\";
#else
        QString dst_file_name = QDir::homePath() + "/oxfold/bigfiletool/downloading/" + obj["file_name"].toString() + ".downloading";
        QString client_cred_path = QDir::toNativeSeparators(QDir::homePath()) + "/oxfold/bigfiletool/client/";
#endif
        QStringList args = {
            "-C",
            url.toStdString().c_str(),
            "-D",
            dst_file_name.toStdString().c_str(),
            "-Z",
            client_cred_path.toStdString().c_str(),
        };

        QProcess *p_http_client =  new QProcess(this);
        QString exe_path;
#if defined(_WIN32)
        exe_path = QDir::toNativeSeparators(QDir::homePath()) + "\\oxfold\\webtool\\oxfold-webtool.exe";
#else
        exe_path = QDir::homePath() + "/oxfold/webtool/oxfold-webtool";
#endif
        qDebug("%s %s %s %s %s", exe_path.toStdString().c_str(), args[0].toStdString().c_str(), args[1].toStdString().c_str(), args[2].toStdString().c_str(), args[3].toStdString().c_str() );
        p_http_client->setProgram(exe_path);
        p_http_client->setArguments(args);
        //p_http_client->start();

        //connect(p_http_client, SIGNAL(readyReadStandardOutput()), this, SLOT(rightMessage()) );
        connect(p_http_client, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [=](int exitCode, QProcess::ExitStatus exitStatus)
        {
            if (exitStatus == 0) //QProcess::NormalExit
            {
                On_client_process_finished(obj["file_name"].toString());
            }
        });

        // save process info for timer operation to update progress.
        Downloading_Task* task = new Downloading_Task;
        task->downloading_process = p_http_client;
        task->downloading_file_name = dst_file_name;
        task->total_len = obj["file_size"].toDouble();
        task->row_in_tableWidge = t->rowCount() - 1 ;
        task->size_in_5s = {c_size, c_size, c_size, c_size, c_size};
        http_client_array.append(task);
    }
}

void Page_downloading::resizeEvent(QResizeEvent *e)
{
    QTableWidget * t = ui->tableWidget;

    QPoint point = ui->tableWidget->mapToParent((ui->tableWidget->rect().topLeft()));
    QSize size = e->size();

    int w = size.width() - point.x();
    int h = size.height() - point.y();

    t->resize(w, h);

    t->setColumnWidth(0, 35);
    t->setColumnWidth(1, t->width()/4); //file name
    t->setColumnWidth(2, 80);           //file size
    t->setColumnWidth(3, t->width()/5); //progress bar
    t->setColumnWidth(4, t->width()/5); //download speed
    t->setColumnWidth(5, t->width()/5); //time left


    QSize  lsize = ui->label->size();
    ui->label->resize(size.width(), lsize.height());

}

void Page_downloading::on_bt_pause_all_clicked()
{
    for (int i=0; i<this->http_client_array.size(); ++i)
    {
        http_client_array.at(i)->downloading_process->kill();
    }
    http_client_array.clear();
}

void Page_downloading::on_bt_start_all_clicked()
{    
    for (int i=0; i<this->http_client_array.size(); ++i)
    {
        Downloading_Task *dt = http_client_array.at(i);
        QProcess *proc = dt->downloading_process;
        proc->start();
    }
    this->start_download_status_timer();
}

void Page_downloading::on_bt_delete_clicked()
{

}

void Page_downloading::on_bt_add_clicked()
{
    Add_link_Dialog *dlg = new Add_link_Dialog(this);
    connect(dlg, SIGNAL(update_parent(QString)), this, SLOT(add_new_download_task(QString)));
    if (dlg->exec() == QDialog::Accepted){}
    delete dlg;
}

void Page_downloading::add_new_download_task(QString data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
    QJsonObject obj;
    if (!doc.isNull() && doc.isObject())
    {
        obj = doc.object();
        // retrieve file into
        QString file_name = obj["file_name"].toString();
        qint64  file_size = obj["file_size"].toDouble();
        QString host_ip   = obj["host_ip"].toString();
        qint64  host_port = obj["host_port"].toInt();

        //qDebug("%s'szie is %s", file_name.toStdString().c_str(), converFileSizeToKBMBGB(file_size).toStdString().c_str());
        // add item into widget
        QTableWidget * t = ui->tableWidget;
        t->insertRow(t->rowCount());
        int n_cols = 4;
        for (int col=0; col!=n_cols; ++col)
        {
            QTableWidgetItem * const i = new QTableWidgetItem;
            i->setFlags(i->flags() & ~Qt::ItemIsEditable);
            if (col == 0 ) {
                //Checkbox
                i->setFlags(
                    Qt::ItemIsSelectable
                  | Qt::ItemIsUserCheckable
                );
                i->setCheckState(Qt::Unchecked);
                t->setItem(t->rowCount()-1, col, i);
            }
            if (col == 1 ) {
                i->setText(file_name);
                t->setItem(t->rowCount()-1, col, i);
            }
            if (col == 2 ) {
                i->setText(QString("0KB/") + MyTool::converFileSizeToKBMBGB(file_size));
                t->setItem(t->rowCount()-1, col, i);
            }
            if (col == 3 ) {  //progress bar
                //i->setText("stopped");
                QProgressBar *pgbar = new QProgressBar();
                pgbar->setRange(0,100);
                pgbar->setValue(0);
                t->setCellWidget(t->rowCount()-1, col, pgbar);
            }
            if (col == 4 ) { //下载速度
                i->setText("已暂停");
                t->setItem(t->rowCount()-1, col, i);
            }
            if (col == 5 ) { //剩余时间
                i->setText("0:0:0");
                t->setItem(t->rowCount()-1, col, i);
            }

        }

        QString url = "http://" + host_ip + ":8080" + "/" + file_name;
#if defined(_WIN32)
        QString dst_file_name = QDir::toNativeSeparators(QDir::homePath()) + "\\oxfold\\bigfiletool\\downloading\\" + file_name + ".downloading";
#else
        QString dst_file_name = QDir::homePath() + "/oxfold/bigfiletool/downloading/" + file_name + ".downloading";
#endif
        QStringList args = {
            "-C",
            url.toStdString().c_str(),
            "-D",
            dst_file_name.toStdString().c_str()
        };

        QProcess *p_http_client =  new QProcess(this);
        QString exe_path;
#if defined(_WIN32)
            exe_path = QDir::toNativeSeparators(QDir::homePath()) + "\\oxfold\\CivetWeb.exe";
#else
            exe_path = QDir::homePath() + "/oxfold/CivetWeb";
#endif
        qDebug("%s %s %s %s %s", exe_path.toStdString().c_str(), args[0].toStdString().c_str(), args[1].toStdString().c_str(), args[2].toStdString().c_str(), args[3].toStdString().c_str() );

        p_http_client->setProgram(exe_path);
        p_http_client->setArguments(args);
        //p_http_client->start();

        //connect(p_http_client, SIGNAL(readyReadStandardOutput()), this, SLOT(rightMessage()) );
        connect(p_http_client, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [=](int exitCode, QProcess::ExitStatus exitStatus)
        {
            if (exitStatus == 0) //QProcess::NormalExit
            {
                On_client_process_finished(file_name);
            }
        });

        this->m_Timer->stop();

        Downloading_Task* task = new Downloading_Task;
        task->downloading_process = p_http_client;
        task->downloading_file_name = dst_file_name;
        task->total_len = obj["file_size"].toDouble();
        task->row_in_tableWidge = t->rowCount();
        task->size_in_5s = {0, 0, 0, 0, 0};
        http_client_array.append(task);

        start_download_status_timer();
    }
}

void Page_downloading::On_client_process_finished(QString fname)
{
#if defined(_WIN32)
    QString old_file = QDir::toNativeSeparators(QDir::homePath()) + "\\oxfold\\bigfiletool\\downloading\\" + fname + ".downloading";
    QString new_file = QDir::toNativeSeparators(QDir::homePath()) + "\\oxfold\\bigfiletool\\downloaded\\" + fname;
    QString old_info_file = QDir::toNativeSeparators(QDir::homePath()) + "\\oxfold\\bigfiletool\\downloading\\" + fname + ".info";
    BOOL flag = MoveFileExA(
       old_file.toStdString().c_str(),
       new_file.toStdString().c_str(),
       MOVEFILE_COPY_ALLOWED
    );
#else
    QString old_file = QDir::homePath() + "/oxfold/bigfiletool/downloading/" + fname + ".downloading";
    QString new_file = QDir::homePath() + "/oxfold/bigfiletool/downloaded/" + fname;
    QString old_info_file = QDir::homePath() + "/oxfold/bigfiletool/downloading/" + fname + ".info";
    QFile::rename(old_file, new_file);
#endif
    // remove downloading info
    QFile(old_info_file).remove();

    // remove tableWidget Item in tableWidget
    // to be done.
}

void Page_downloading::update_download_task(QString fname,quint64 len, quint64 total)
{
    qDebug("Page_downloading: %s %lld / %lld bytes => %d%% complete\n", fname.toStdString().c_str(), len, total, (int)(len*100/total));
}


void Page_downloading::on_tableWidget_itemClicked(QTableWidgetItem *item)
{
    QTableWidget * t = ui->tableWidget;
    const int n_rows = t->rowCount();
    for (int row=0; row!=n_rows; ++row)
    {
        QTableWidgetItem *  i = t->item(row, 0);
        i->setCheckState(Qt::Unchecked);
    }

    int row = item->row();
    QTableWidgetItem *  i = t->item(row, 0);
    Qt::CheckState s = i->checkState();
    if (s == Qt::Unchecked) {
        i->setCheckState(Qt::Checked);
        ui->bt_delete->setVisible(true);
        ui->bt_start_one->setVisible(true);
        ui->bt_pause_one->setVisible(true);
    }else {
        i->setCheckState(Qt::Unchecked);
        ui->bt_delete->setVisible(false);
        ui->bt_start_one->setVisible(false);
        ui->bt_pause_one->setVisible(false);
    }
}

void Page_downloading::on_bt_start_one_clicked()
{

}

void Page_downloading::on_bt_pause_one_clicked()
{

}
