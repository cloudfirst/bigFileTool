#include "page_shared.h"
#include "ui_page_shared.h"
#include <QResizeEvent>
#include <QFileDialog>
#include <QDateTime>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>

#include "sharing_dialog.h"
#include "encrypt/simple_encrypt.h"
#include "mytool.h"

#if defined(_WIN32)
#include "Windows.h"
#endif


Page_shared::Page_shared(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Page_shared)
{
    ui->setupUi(this);
    ui->bt_delete_share->setVisible(false);
    ui->bt_share_file->setVisible(false);

    b_start_webserver_auto = true;
    b_destroy = false;

    this->init_table();

    this->p_http_server = new QProcess(this);

    if (b_start_webserver_auto) {
        QString exe_path;
    #if defined(_WIN32)
         exe_path = QDir::toNativeSeparators(QDir::homePath()) + "\\oxfold\\webtool\\oxfold-webtool.exe";
         QStringList args = {
             "-document_root",
             QDir::toNativeSeparators(QDir::homePath()) + "\\oxfold\\bigfiletool\\shared"
         };
    #else
        exe_path = QDir::homePath() + "/oxfold/webtool/oxfold-webtool";
        QStringList args = {
             "-document_root",
             QDir::homePath() + "/oxfold/bigfiletool/shared"
        };
    #endif

        p_http_server->setProgram(exe_path);
        p_http_server->setArguments(args);
        p_http_server->start();

        connect(p_http_server, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [=](int exitCode, QProcess::ExitStatus exitStatus)
        {
            On_http_server_finished();
        });
    }
}

void Page_shared::On_http_server_finished()
{
    if (b_destroy) return;

    if (p_http_server->state() != QProcess::Running) {
        p_http_server->start();
    }
}


Page_shared::~Page_shared()
{
    b_destroy = true;
    delete ui;
    p_http_server->kill();
}

void Page_shared::init_table()
{
    //QStringList header_title = { " ","文件名", "大小", "修改时间", "" };
    QTableWidget * t = ui->shared_file_tableWidget;

    t->verticalHeader()->setVisible(false);
    //t->setHorizontalHeaderLabels(header_title);
    //t->horizontalHeader()->defaultAlignment()
    //t->resizeColumnsToContents();
    t->setSelectionBehavior(QAbstractItemView::SelectRows);
    t->setSelectionMode( QAbstractItemView::SingleSelection );

    // init data
    int n_cols = 4;
    QDir dir1(QDir::homePath() + "/oxfold/bigfiletool/shared");
    QFileInfoList list = dir1.entryInfoList();
    for (int i = 0; i < list.size(); ++i)
    {
        QFileInfo fileInfo = list.at(i);
        if (fileInfo.isDir())
            continue;

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
                i->setText(fileInfo.fileName());
            }
            if (col == 2 ) {
                i->setText( MyTool::converFileSizeToKBMBGB(fileInfo.size()));
            }
            if (col == 3 ) {
                i->setText(fileInfo.lastModified().toString("yyyy.MM.dd hh:mm"));
            }
            t->setItem(t->rowCount()-1, col, i);
        }
    }
}

void Page_shared::resizeEvent(QResizeEvent *e)
{
    QTableWidget * t = ui->shared_file_tableWidget;

    QPoint point = ui->shared_file_tableWidget->mapToParent((ui->shared_file_tableWidget->rect().topLeft()));
    QSize size = e->size();

    int w = size.width() - point.x();
    int h = size.height() - point.y();

    t->resize(w, h);

    t->setColumnWidth(0, 35);
    t->setColumnWidth(1, t->width()/2);
    t->setColumnWidth(2, t->width()/6);
    t->setColumnWidth(3, t->width()/3);

    QSize  lsize = ui->label->size();
    ui->label->resize(size.width(), lsize.height());

}

void Page_shared::on_shared_file_tableWidget_itemClicked(QTableWidgetItem *item)
{
    QTableWidget * t = ui->shared_file_tableWidget;
    int clicked_row = item->row();
    int pre_row = -1;
    const int n_rows = t->rowCount();
    QItemSelectionModel *select = t->selectionModel();
    QModelIndexList  rows;

    // get previously selected row
    for (int row=0; row!=n_rows; ++row)
    {
        QTableWidgetItem *  i = t->item(row, 0);
        Qt::CheckState s = i->checkState();
        if (s == Qt::Checked) {
            pre_row = row;
            break;
        }
    }

    if (select->hasSelection())  // select row before
    {
        if (pre_row == clicked_row) {  //de-select this row
            QTableWidgetItem *  i = t->item(clicked_row, 0);
            Qt::CheckState s = i->checkState();
            if (s == Qt::Unchecked) {
                i->setCheckState(Qt::Checked);
                ui->bt_delete_share->setVisible(true);
                ui->bt_share_file->setVisible(true);
            } else {
                i->setCheckState(Qt::Unchecked);
                t->selectionModel()->clearSelection();
                ui->bt_delete_share->setVisible(false);
                ui->bt_share_file->setVisible(false);
            }
        } else {  // select a new row
            // otherwise highlight the clicked row.
            if (pre_row != -1) {
                 QTableWidgetItem *  j = t->item(pre_row, 0);
                 j->setCheckState(Qt::Unchecked);
            }
            QTableWidgetItem *  i = t->item(clicked_row, 0);
            i->setCheckState(Qt::Checked);
            ui->bt_delete_share->setVisible(true);
            ui->bt_share_file->setVisible(true);
        }
    }
}

void Page_shared::mklink(QString target, QString link)
{
#if defined (_WIN32)
    QString _target = QDir::toNativeSeparators(target);
    QString _link   = QDir::toNativeSeparators(link);
    BOOL fCreatedLink = CreateHardLinkA( _link.toStdString().c_str(),
                                        _target.toStdString().c_str(),
                                        NULL ); // reserved, must be NULL

    if ( fCreatedLink == FALSE )
    {
        qDebug("create hadr link failed in windows : %s", _target.toStdString().c_str());
    }
#else
    QFile::link(target, link);
#endif
}

// the default dir for shared files are located in ~/oxfold/bigfiletool/shared、
void Page_shared::on_bt_add_share_file_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Select File"), ".", tr("All Files(*.*)"));
    if(path.length() == 0) {

    } else {
        QFileInfo info(path);
        QString     afp   = info.absoluteFilePath();
        QString     name  = info.fileName();
        qint64      size  = info.size();
        QDateTime   lastmodified = info.lastModified();

        //create symlink at ~/oxfold/bigfiletool/shared
        mklink(afp, QDir::homePath() + "/oxfold/bigfiletool/shared/" + name);

        //add item to tableWidget
        QTableWidget * t = ui->shared_file_tableWidget;
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
                i->setText(name);
            }
            if (col == 2 ) {
                i->setText(MyTool::converFileSizeToKBMBGB(size));
            }
            if (col == 3 ) {
                i->setText(lastmodified.toString("yyyy.MM.dd hh:mm"));
            }
            t->setItem(t->rowCount()-1, col, i);
        }

    }
}


qint64 getFileSize(QString shared_file_name)
{
#if defined (_WIN32)
    QString file_path = QDir::toNativeSeparators(QDir::homePath()) + "\\oxfold\\bigfiletool\\shared\\" +  shared_file_name;
#else
    QString file_path = QDir::homePath() + "/oxfold/bigfiletool/shared/" +  shared_file_name;
#endif
    QFileInfo info(file_path);
    return info.size();
}

QString simple_encrypt(QString input, QString pass_word)
{
    quint64 key = pass_word.toULongLong();
    SimpleCrypt processSimpleCrypt(key);

    QString output = processSimpleCrypt.encryptToString(input);
    return output;
}

QString my_randString(int len)
{
    QString str;
    str.resize(len);
    for (int s = 0; s < len ; ++s)
        str[s] = QChar('A' + char(qrand() % ('Z' - 'A')));

    return str;
}


// sharing link looks like http://ip:8080/filename.ext
void Page_shared::on_bt_share_file_clicked()
{
    QString host_ip = MyTool::getNodeIPV4();
    if (host_ip == "0.0.0.0") {
        QMessageBox msgBox;
        msgBox.setText("请稍候，等内置的web服务启动后再点击分享。");
        msgBox.exec();
        return;
    }

    int     port = 8080;
    QString file_name;
    qint64  file_size;
    QString pass_word = my_randString(6);

    QTableWidget * t = ui->shared_file_tableWidget;
    const int n_rows = t->rowCount();
    for (int row=0; row!=n_rows; ++row)
    {
        QTableWidgetItem *  i = t->item(row, 0);
        Qt::CheckState s = i->checkState();
        if (s == Qt::Checked) {
            i = t->item(row, 1);
            file_name = i->text();
            break;
        }
    }

    file_size = getFileSize(file_name);

    QString ext_link = "http://" + host_ip + ":8080/" + file_name;
    qDebug("external link : %s", ext_link.toStdString().c_str());

    QJsonObject external_link_data;
    external_link_data["host_ip"]   = host_ip.toStdString().c_str();
    external_link_data["host_port"] = port;
    external_link_data["file_name"] = file_name.toStdString().c_str();
    external_link_data["file_size"] = file_size;

    QByteArray byteArray;
    byteArray = QJsonDocument(external_link_data).toJson();
    QString link = QString(byteArray.toStdString().c_str()).simplified();
    qDebug("external_link_data byte array = %s", link.toStdString().c_str());

    link = simple_encrypt(link, pass_word);
    qDebug("encrypted external_link_data is : %s", link.toStdString().c_str());

    sharing_Dialog dlg(this);
    dlg.init_data(link, pass_word);
    dlg.exec();
}
