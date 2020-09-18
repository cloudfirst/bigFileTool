#ifndef PAGE_DOWNLOADING_H
#define PAGE_DOWNLOADING_H

#include <QWidget>
#include <QTableWidgetItem>
#include "http_thread.h"
#include <QList>
#include <QProcess>
#include <QTimer>

namespace Ui {
class Page_downloading;
}

struct Downloading_Task {
    QProcess *downloading_process;
    QString  downloading_file_name;
    QList<uint64_t> size_in_5s;
    uint64_t total_len;
    int      row_in_tableWidge;
};

class Page_downloading : public QWidget
{
    Q_OBJECT

public:
    explicit Page_downloading(QWidget *parent = nullptr);
    ~Page_downloading();

    void init_table();
    void On_client_process_finished(QString fname);

protected:
    virtual void resizeEvent(QResizeEvent *e) override;
    void start_download_status_timer();

public slots:
    void MyTimerSlot();
    void add_new_download_task(QString data);
    void update_download_task(QString fname, quint64 len, quint64 total);

private slots:
    void on_bt_pause_all_clicked();

    void on_bt_start_all_clicked();

    void on_bt_delete_clicked();

    void on_bt_add_clicked();

    void on_tableWidget_itemClicked(QTableWidgetItem *item);

    void on_bt_start_one_clicked();

    void on_bt_pause_one_clicked();

private:
    Ui::Page_downloading *ui;
    QList<Downloading_Task*> http_client_array;
    QTimer * m_Timer;

};

#endif // PAGE_DOWNLOADING_H
