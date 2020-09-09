#ifndef PAGE_DOWNLOADING_H
#define PAGE_DOWNLOADING_H

#include <QWidget>
#include <QTableWidgetItem>
#include "http_thread.h"
#include <QList>

namespace Ui {
class Page_downloading;
}

class Page_downloading : public QWidget
{
    Q_OBJECT

public:
    explicit Page_downloading(QWidget *parent = nullptr);
    ~Page_downloading();

    void init_table();

protected:
    virtual void resizeEvent(QResizeEvent *e) override;

public slots:
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

};

#endif // PAGE_DOWNLOADING_H
