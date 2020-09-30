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
#include <QMessageBox>

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
    this->max_download_tasks = 1;
    this->b_destroy = false;

    ui->bt_delete->setVisible(false);
    ui->bt_start_one->setVisible(false);
    ui->bt_pause_one->setVisible(false);

    ui->bt_pause_all->setVisible(false);
    ui->bt_start_all->setVisible(false);
}


Page_downloading::~Page_downloading()
{
    b_destroy = true;
    delete ui;
    this->stop_download_status_timer();
    for (int i=0; i<this->http_client_array.size(); ++i)
    {
        http_client_array.at(i)->downloading_process->kill();
        delete  http_client_array.at(i);
    }
}

void Page_downloading::stop_download_status_timer()
{
    if (m_Timer != NULL) {
        m_Timer->stop();
        delete m_Timer;
        m_Timer = NULL;
    }
}

void Page_downloading::start_download_status_timer()
{
    if (m_Timer == NULL) {
        m_Timer = new QTimer(this);
        connect(m_Timer, SIGNAL(timeout()),this, SLOT(MyTimerSlot()));
    }
    m_Timer->start(1000*3);
}

QString convertSencond2HHMMSS(uint64_t time)
{
    int hour = time/3600;
    if (hour > 100) hour = 100;

    time = time%3600;
    int min = time/60;
    time = time%60;
    int sec = time;

    return QString::number(hour) + ":" + QString::number(min) + ":" + QString::number(sec);
}

int Page_downloading::getNumberOfRuningTasks()
{
    int nRun = 0;
    for (int i=0; i<this->http_client_array.size(); ++i)
    {
        QProcess *proc = http_client_array.at(i)->downloading_process;
        if (proc->state() == QProcess::Running) {
            nRun += 1;
        }
    }
    return nRun;
}

uint64_t getCurrentFileSize(QString fname)
{
    QString df(MyTool::getDownloadingDir() + fname + ".downloading");
    QFileInfo  info(df);
    return info.size();
}

int Page_downloading::getTableRowByName(QString name)
{
    QTableWidget * t = ui->tableWidget;
    QTableWidgetItem *item;
    QString fname;
    int rows = t->rowCount();

    int i;
    for (i=0; i<rows; i++)
    {
       item = t->item(i, 1);
       fname = item->text();
       if (fname == name) {
           return i;
       }
    }
    return -1;
}

void Page_downloading::MyTimerSlot()
{
    QTableWidget * t = ui->tableWidget;
    QProgressBar *mypgb;
    QTableWidgetItem *item;

    for (int i=0; i<this->http_client_array.size(); ++i)
    {
        Downloading_Task *dt = http_client_array.at(i);
        QProcess *proc = dt->downloading_process;
        if (proc->state() != QProcess::Running)
            continue;

        QString fname = dt->downloading_file_name;
        uint64_t new_size = getCurrentFileSize(dt->downloading_file_percentage_name);

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
        int row = getTableRowByName(fname);
        if (row == -1)
            continue;

        item = t->item(row, 2);
        QString _f_len_s = MyTool::converFileSizeToKBMBGB(new_size) + " / " + \
                MyTool::converFileSizeToKBMBGB(dt->total_len);
        if (item != NULL) {
            item->setText(_f_len_s);
        }

        mypgb = (QProgressBar*)t->cellWidget(row, 3); // update progress bar
        if (mypgb !=NULL) {
            mypgb->setValue(percentage);
        }


        item = t->item(row, 4);  // update speed
        QString _txt = MyTool::converFileSizeToKBMBGB((qint64)bps) + "/s" ;
        if (item != NULL) {
            item->setText(_txt);
        }


        item = t->item(row, 5);  // update time left
        if (item != NULL) {
            item->setText("剩余 " +  convertSencond2HHMMSS(letf_seconds));
        }
    }
}

void Page_downloading::init_table()
{
    QTableWidget * t = ui->tableWidget;

    t->verticalHeader()->setVisible(false);
    t->setSelectionBehavior(QAbstractItemView::SelectRows);
    t->setSelectionMode( QAbstractItemView::SingleSelection );

    // init data
    int n_cols = 6;
    QDir dir1(MyTool::getDownloadingDir());
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

        QByteArray ba = QUrl::toPercentEncoding(obj["file_name"].toString(), "", "");
        uint64_t  c_size = getCurrentFileSize(QString(ba));

        for (int col=0; col!=n_cols; ++col)
        {
            QTableWidgetItem * const i = new QTableWidgetItem();
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
                i->setText( MyTool::converFileSizeToKBMBGB(c_size) + \
                            " / " + \
                            MyTool::converFileSizeToKBMBGB(obj["file_size"].toDouble()));
                t->setItem(t->rowCount()-1, col, i);
            }
            if (col == 3 ) {  //progress bar
                QProgressBar *pgbar = new QProgressBar();
                pgbar->setRange(0,100);

                int percentage = c_size * 100 / obj["file_size"].toDouble();
                if (percentage > 100) {
                    percentage = 100;
                }
                pgbar->setValue(percentage);

                t->setCellWidget(t->rowCount()-1, col, pgbar);
            }
            if (col == 4 ) { //下载速度
                i->setText("已暂停");
                t->setItem(t->rowCount()-1, col, i);
            }
            if (col == 5 ) { //剩余时间
                i->setText("剩余 0:0:0");
                t->setItem(t->rowCount()-1, col, i);
            }

        }


        QString url = "http://" + obj["host_ip"].toString() + ":8080" + "/" + ba;
        QString dst_file_name = MyTool::getDownloadingDir() + ba + ".downloading";
        QString client_cred_path = MyTool::getClientCredDir();
        QStringList args = {
            "-C",
            url,
            "-D",
            dst_file_name,
            "-Z",
            client_cred_path,
        };

        QProcess *p_http_client =  new QProcess(this);
        QString exe_path = MyTool::getOxfoldWebTool();

        p_http_client->setProgram(exe_path);
        p_http_client->setArguments(args);

        connect(p_http_client, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [=](int exitCode, QProcess::ExitStatus exitStatus)
        {
            On_client_process_finished(obj["file_name"].toString());
        });

        // save process info for timer operation to update progress.
        Downloading_Task* task = new Downloading_Task;
        task->downloading_process = p_http_client;
        task->proc_args = args;
        task->exe_path  = exe_path;
        task->downloading_file_name = obj["file_name"].toString();
        task->downloading_file_percentage_name = ba;
        task->total_len = obj["file_size"].toDouble();
        task->size_in_5s = {c_size, c_size, c_size, c_size, c_size};

        task->is_manually_stopped = false;
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
    t->setColumnWidth(1, t->width()/5); //file name
    t->setColumnWidth(2, t->width()/5); //file size
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
        Downloading_Task *dt = http_client_array.at(i);
        dt->downloading_process->kill();
        dt->is_manually_stopped = true;
    }
    this->stop_download_status_timer();
}

void Page_downloading::on_bt_start_all_clicked()
{    
    int nRun = this->getNumberOfRuningTasks();
    for (int i=0; i<this->http_client_array.size(); ++i)
    {
        Downloading_Task *dt = http_client_array.at(i);
        QProcess *proc = dt->downloading_process;

        if ( nRun < this->max_download_tasks && proc->state() != QProcess::Running) {
            proc->start(dt->exe_path, dt->proc_args);
            nRun += 1;
        }
        dt->is_manually_stopped = false;
    }
    this->start_download_status_timer();
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

        // add item into widget
        QTableWidget * t = ui->tableWidget;
        t->insertRow(t->rowCount());

        int n_cols = 6;
        for (int col=0; col!=n_cols; ++col)
        {
            QTableWidgetItem * const i = new QTableWidgetItem();
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
                i->setText("剩余 0:0:0");
                t->setItem(t->rowCount()-1, col, i);
            }

        }

        QByteArray ba = QUrl::toPercentEncoding(file_name, "", "");
        QString url = "http://" + host_ip + ":8080" + "/" + ba;
        QString dst_file_name = MyTool::getDownloadingDir() + ba + ".downloading";
        QString client_cred_path = MyTool::getClientCredDir();
        QStringList args = {
            "-C",
            url,
            "-D",
            dst_file_name,
            "-Z",
            client_cred_path,
        };

        QProcess *p_http_client =  new QProcess(this);
        QString exe_path = MyTool::getOxfoldWebTool();

        p_http_client->setProgram(exe_path);
        p_http_client->setArguments(args);

        connect(p_http_client, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [=](int exitCode, QProcess::ExitStatus exitStatus)
        {
            On_client_process_finished(file_name);
        });

        Downloading_Task* task = new Downloading_Task;
        task->downloading_process = p_http_client;
        task->exe_path  = exe_path;
        task->proc_args = args;
        task->downloading_file_name = file_name;
        task->downloading_file_percentage_name = ba;
        task->total_len = obj["file_size"].toDouble();
        task->size_in_5s = {0, 0, 0, 0, 0};
        task->is_manually_stopped = false;
        http_client_array.append(task);

    }
}


// if process terminated
// if the file length < total_len and is_manually_stop == false, restart it.
// if the file length >= tootal_len,  start post downloading task.
void Page_downloading::On_client_process_finished(QString fname)
{
    if (b_destroy) return;

    Downloading_Task *dt = NULL;
    uint64_t new_size;
    QString percentage_name;
    int i=0;
    for (i=0; i<this->http_client_array.size(); ++i)
    {
        dt = http_client_array.at(i);
        if (dt->downloading_file_name == fname) {
            percentage_name = dt->downloading_file_percentage_name;
            new_size = getCurrentFileSize(percentage_name);
            break;
        }
    }

    if (dt == NULL) return;

    if (new_size < dt->total_len) {   // try to restart http_client_process
         QProcess *proc = dt->downloading_process;
         if (proc->state() != QProcess::Running &&
             getNumberOfRuningTasks() < max_download_tasks &&
             dt->is_manually_stopped == false)
         {
             proc->start();
         }
    } else {  // downloading is finished, start post_downloading tasks.
        QString old_file = MyTool::getDownloadingDir() + percentage_name + ".downloading";
        QString new_file = MyTool::getDownloadedDir() + fname;
        QString old_info_file  = MyTool::getDownloadingDir() + percentage_name + ".info";

    #if defined(_WIN32)
        BOOL flag = MoveFileExW(
           (LPCWSTR)old_file.toStdWString().c_str(),
           (LPCWSTR)new_file.toStdWString().c_str(),
           MOVEFILE_COPY_ALLOWED
        );
    #else
        QFile::rename(old_file, new_file);
    #endif

        // remove downloading info
        QFile(old_info_file).remove();

        QTableWidget * t = ui->tableWidget;
        int row = getTableRowByName(fname);
        if (row >= 0) {
             t->removeRow(row);
        }
        http_client_array.removeAt(i);

        emit download_task_finished(fname);
    }
}

void Page_downloading::on_tableWidget_itemClicked(QTableWidgetItem *item)
{
    QTableWidget * t = ui->tableWidget;
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
                ui->bt_delete->setVisible(true);
                ui->bt_start_one->setVisible(true);
                ui->bt_pause_one->setVisible(true);
            } else {
                i->setCheckState(Qt::Unchecked);
                t->selectionModel()->clearSelection();
                ui->bt_delete->setVisible(false);
                ui->bt_start_one->setVisible(false);
                ui->bt_pause_one->setVisible(false);
            }
        } else {  // select a new row
            // otherwise highlight the clicked row.
            if (pre_row != -1) {
                 QTableWidgetItem *  j = t->item(pre_row, 0);
                 j->setCheckState(Qt::Unchecked);
            }
            QTableWidgetItem *  i = t->item(clicked_row, 0);
            i->setCheckState(Qt::Checked);
            ui->bt_delete->setVisible(true);
            ui->bt_start_one->setVisible(true);
            ui->bt_pause_one->setVisible(true);
        }
    }
}

void Page_downloading::on_bt_delete_clicked()
{
    QTableWidget * t = ui->tableWidget;
    QItemSelectionModel *select = t->selectionModel();
    QModelIndexList  rows;
    QString fname;
    QString percentage_name;

    if (select->hasSelection())
    {
        rows = select->selectedRows();
        int row = rows.at(0).row();

        //2 delete task in tasks
        this->stop_download_status_timer();
            Downloading_Task *dt = http_client_array.at(row);
            dt->downloading_process->kill();
            fname = dt->downloading_file_name;
            //1 delete row in tableWidget
            int _row = getTableRowByName(fname);
            if (_row >= 0) {
                 t->removeRow(_row);
            }
            percentage_name = dt->downloading_file_percentage_name;
            http_client_array.removeAt(row);
        this->start_download_status_timer();

        //3 delete files in downloading directory
        QString  df(MyTool::getDownloadingDir() + percentage_name + ".downloading");
        QString inf(MyTool::getDownloadingDir() + percentage_name + ".info");
        QFile::remove(df);
        QFile::remove(inf);
    } else {
        QMessageBox msgBox;
        msgBox.setText("请先选择一个下载任务!");
        msgBox.exec();
    }
}

void Page_downloading::on_bt_start_one_clicked()
{
    QTableWidget * t = ui->tableWidget;
    QItemSelectionModel *select = t->selectionModel();
    QModelIndexList  rows;

    if (select->hasSelection())
    {
        rows = select->selectedRows();
        int row = rows.at(0).row();
        Downloading_Task *dt = this->http_client_array.at(row);
        QProcess *proc = dt->downloading_process;
        if (this->getNumberOfRuningTasks() < this->max_download_tasks)
        {
            proc->start(dt->exe_path, dt->proc_args);
            start_download_status_timer();
        } else {
            QMessageBox msgBox;
            msgBox.setText("当前下载任务数已经满了,请等候!");
            msgBox.exec();
        }
        dt->is_manually_stopped = false;
    } else {
        QMessageBox msgBox;
        msgBox.setText("请先选择一个下载任务!");
        msgBox.exec();
    }
}

void Page_downloading::on_bt_pause_one_clicked()
{
    QTableWidget * t = ui->tableWidget;
    QItemSelectionModel *select = t->selectionModel();
    QModelIndexList  rows;

    if (select->hasSelection())
    {
        rows = select->selectedRows();
        int row = rows.at(0).row();

        Downloading_Task *dt = this->http_client_array.at(row);
        QProcess *proc = dt->downloading_process;
        proc->kill();
        dt->is_manually_stopped = true;

        QTableWidgetItem *item;
        item = t->item(row, 4);
        item->setText("已暂停");
        t->setItem(t->rowCount()-1, 4, item);

        item = t->item(row, 5);
        item->setText("剩余 0:0:0");
        t->setItem(t->rowCount()-1, 5, item);
    } else {
        QMessageBox msgBox;
        msgBox.setText("请先选择一个下载任务!");
        msgBox.exec();
    }
}
