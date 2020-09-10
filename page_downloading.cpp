#include "page_downloading.h"
#include "ui_page_downloading.h"
#include <QResizeEvent>
#include <QFileDialog>
#include <QDateTime>
#include <QDebug>
#include "add_link_dialog.h"
#include <QJsonDocument>
#include <QJsonObject>
#include "http_thread.h"
#include <QJsonArray>
#include "mytool.h"

Page_downloading::Page_downloading(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Page_downloading)
{
    ui->setupUi(this);
    init_table();
}

Page_downloading::~Page_downloading()
{
    delete ui;
    for (int i=0; i<this->http_client_array.size(); ++i)
    {
        http_client_array.at(i)->kill();
    }
}

void Page_downloading::init_table()
{
    QTableWidget * t = ui->tableWidget;

    t->verticalHeader()->setVisible(false);
    t->setSelectionBehavior(QAbstractItemView::SelectRows);
    t->setSelectionMode( QAbstractItemView::SingleSelection );

    // init data
    int n_cols = 4;
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
            }
            if (col == 1 ) {
                i->setText(obj["file_name"].toString());
            }
            if (col == 2 ) {
                i->setText(MyTool::converFileSizeToKBMBGB(obj["file_size"].toDouble()));
            }
            if (col == 3 ) {
                i->setText("stopped");
            }
            t->setItem(t->rowCount()-1, col, i);
        }

        QString url = "http://" + obj["host_ip"].toString() + ":8080" + "/" +obj["file_name"].toString();
#if defined(_WIN32)
        QString dst_file_name = QDir::toNativeSeparators(QDir::homePath()) + "\\oxfold\\bigfiletool\\downloading\\" + obj["file_name"].toString() + ".downloading";
#else
        QString dst_file_name = QDir::homePath() + "/oxfold/bigfiletool/downloading/" + obj["file_name"].toString() + ".downloading";
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
        p_http_client->start(exe_path, args);
        http_client_array.append(p_http_client);
        //connect(p_http_client, SIGNAL(readyReadStandardOutput()), this, SLOT(rightMessage()) );
        connect(p_http_client, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [=](int exitCode, QProcess::ExitStatus exitStatus)
        {
            if (exitStatus == 0) //QProcess::NormalExit
            {
                On_client_process_finished(obj["file_name"].toString());
            }
        });
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
    t->setColumnWidth(1, t->width()/3);
    t->setColumnWidth(2, t->width()/3);
    t->setColumnWidth(3, t->width()/3);

    QSize  lsize = ui->label->size();
    ui->label->resize(size.width(), lsize.height());

}

void Page_downloading::on_bt_pause_all_clicked()
{
    for (int i=0; i<this->http_client_array.size(); ++i)
    {
        http_client_array.at(i)->kill();
    }
    http_client_array.clear();
}

void Page_downloading::on_bt_start_all_clicked()
{
    QTableWidget * t = ui->tableWidget;
    t->setRowCount(0);
    init_table();
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
            }
            if (col == 1 ) {
                i->setText(file_name);
            }
            if (col == 2 ) {
                i->setText(QString("0KB/") + MyTool::converFileSizeToKBMBGB(file_size));
            }
            if (col == 3 ) {
                i->setText(QString("0%"));
            }
            t->setItem(t->rowCount()-1, col, i);
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
        p_http_client->start(exe_path, args);
        http_client_array.append(p_http_client);
        //connect(p_http_client, SIGNAL(readyReadStandardOutput()), this, SLOT(rightMessage()) );
        connect(p_http_client, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [=](int exitCode, QProcess::ExitStatus exitStatus)
        {
            if (exitStatus == 0) //QProcess::NormalExit
            {
                On_client_process_finished(file_name);
            }
        });
    }
}

void Page_downloading::On_client_process_finished(QString fname)
{
#if defined(_WIN32)
    QString old_file = QDir::toNativeSeparators(QDir::homePath()) + "\\oxfold\\bigfiletool\\downloading\\" + fname + ".downloading";
    QString new_file = QDir::toNativeSeparators(QDir::homePath()) + "\\oxfold\\bigfiletool\\downloaded\\" + fname;
    QString old_info_file = QDir::toNativeSeparators(QDir::homePath()) + "\\oxfold\\bigfiletool\\downloading\\" + fname + ".info";
#else
    QString old_file = QDir::homePath() + "/oxfold/bigfiletool/downloading/" + fname + ".downloading";
    QString new_file = QDir::homePath() + "/oxfold/bigfiletool/downloaded/" + fname;
    QString old_info_file = QDir::homePath() + "/oxfold/bigfiletool/downloading/" + fname + ".info";
#endif

    QFile::rename(old_file, new_file);

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
