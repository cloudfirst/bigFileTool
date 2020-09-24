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
#if defined(_WIN32)
    QString df(QDir::toNativeSeparators(QDir::homePath()) + "\\oxfold\\bigfiletool\\downloading\\" + fname + ".downloading");
#else
    QString df(QDir::homePath() + "/oxfold/bigfiletool/downloading/" + fname + ".downloading");
#endif
    QFileInfo  info(df);
    return info.size();
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
        uint64_t new_size = getCurrentFileSize(dt->downloading_file_name);

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
        int row = dt->row_in_tableWidge;

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

        QString url = "http://" + obj["host_ip"].toString() + ":8080" + "/" +obj["file_name"].toString();
#if defined(_WIN32)
        QString dst_file_name = QDir::toNativeSeparators(QDir::homePath()) + "\\oxfold\\bigfiletool\\downloading\\" + obj["file_name"].toString() + ".downloading";
        QString client_cred_path = QDir::toNativeSeparators(QDir::homePath()) + "\\oxfold\\bigfiletool\\client\\";
#else
        QString dst_file_name = QDir::homePath() + "/oxfold/bigfiletool/downloading/" + obj["file_name"].toString() + ".downloading";
        QString client_cred_path = QDir::homePath() + "/oxfold/bigfiletool/client/";
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

        //connect(p_http_client, SIGNAL(readyReadStandardOutput()), this, SLOT(rightMessage()) );
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
        task->total_len = obj["file_size"].toDouble();
        task->row_in_tableWidge = t->rowCount() - 1 ;
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

        QString url = "http://" + host_ip + ":8080" + "/" + file_name;
#if defined(_WIN32)
        QString dst_file_name = QDir::toNativeSeparators(QDir::homePath()) + "\\oxfold\\bigfiletool\\downloading\\" + file_name + ".downloading";
        QString client_cred_path = QDir::toNativeSeparators(QDir::homePath()) + "\\oxfold\\bigfiletool\\client\\";
#else
        QString dst_file_name = QDir::homePath() + "/oxfold/bigfiletool/downloading/" + file_name + ".downloading";
        QString client_cred_path = QDir::homePath() + "/oxfold/bigfiletool/client/";
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

        //connect(p_http_client, SIGNAL(readyReadStandardOutput()), this, SLOT(rightMessage()) );
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
        task->total_len = obj["file_size"].toDouble();
        task->row_in_tableWidge = t->rowCount() - 1;
        task->size_in_5s = {0, 0, 0, 0, 0};
        task->is_manually_stopped = false;
        http_client_array.append(task);

//        if (this->getNumberOfRuningTasks() < this->max_download_tasks) {
//            p_http_client->start();
//            start_download_status_timer();
//        }
    }
}

// if process terminated
// if the file length < total_len and is_manually_stop == false, restart it.
// if the file length >= tootal_len,  start post downloading task.
void Page_downloading::On_client_process_finished(QString fname)
{
    if (b_destroy) return;

    Downloading_Task *dt = NULL;
    uint64_t new_size = getCurrentFileSize(fname);
    int i=0;
    for (i=0; i<this->http_client_array.size(); ++i)
    {
        dt = http_client_array.at(i);
        if (dt->downloading_file_name == fname) {
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

        QTableWidget * t = ui->tableWidget;
        t->removeRow(dt->row_in_tableWidge);
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

    if (select->hasSelection())
    {
        rows = select->selectedRows();
        int row = rows.at(0).row();

        //1 delete row in tableWidget
        t->removeRow(row);

        //2 delete task in tasks
        this->stop_download_status_timer();
            Downloading_Task *dt = http_client_array.at(row);
            dt->downloading_process->kill();
            fname = dt->downloading_file_name;
            http_client_array.removeAt(row);
        this->start_download_status_timer();

        //3 delete files in downloading directory
    #if defined(_WIN32)
        QString df(QDir::toNativeSeparators(QDir::homePath()) + "\\oxfold\\bigfiletool\\downloading\\" + fname + ".downloading");
        QString inf(QDir::toNativeSeparators(QDir::homePath()) + "\\oxfold\\bigfiletool\\downloading\\" + fname + ".info");
    #else
        QString df(QDir::homePath() + "/oxfold/bigfiletool/downloading/" + fname + ".downloading");
        QString inf(QDir::toNativeSeparators(QDir::homePath()) + "/oxfold/bigfiletool/downloading/" + fname + ".info");
    #endif
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
    } else {
        QMessageBox msgBox;
        msgBox.setText("请先选择一个下载任务!");
        msgBox.exec();
    }
}
