#include "page_shared.h"
#include "ui_page_shared.h"
#include <QResizeEvent>
#include <QFileDialog>
#include <QDateTime>
#include "oxfold_wrapper.h"
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include "sharing_dialog.h"

Page_shared::Page_shared(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Page_shared)
{
    ui->setupUi(this);
    ui->bt_delete_share->setVisible(false);
    ui->bt_share_file->setVisible(false);

    this->init_table();
}

Page_shared::~Page_shared()
{
    delete ui;
}



void Page_shared::init_table()
{
    QStringList header_title = { " ","文件名", "大小", "修改时间", "" };
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
                i->setText(converFileSizeToKBMBGB(fileInfo.size()));
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
        ui->bt_delete_share->setVisible(true);
        ui->bt_share_file->setVisible(true);
    }else {
        i->setCheckState(Qt::Unchecked);
        ui->bt_delete_share->setVisible(false);
        ui->bt_share_file->setVisible(false);
    }

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
        QFile::link(afp, QDir::homePath() + "/oxfold/bigfiletool/shared/" + name);

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
                i->setText(converFileSizeToKBMBGB(size));
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
    QString file_path = QDir::homePath() + "/oxfold/bigfiletool/shared/" +  shared_file_name;
    QFileInfo info(file_path);

    return info.size();
}

// sharing link looks like http://ip:8080/filename.ext
void Page_shared::on_bt_share_file_clicked()
{
    QString host_ip = getNodeIPV4();
    int     port = 8080;
    QString file_name;
    qint64  file_size;
    QString pass_word = "abcd";

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

   sharing_Dialog dlg(this);
   dlg.init_data(link, pass_word);
   dlg.exec();
}
