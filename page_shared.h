#ifndef PAGE_SHARED_H
#define PAGE_SHARED_H

#include <QWidget>
#include <QTableWidgetItem>
#include <QProcess>
#include <QTimer>

namespace Ui {
class Page_shared;
}

class Page_shared : public QWidget
{
    Q_OBJECT

public:
    explicit Page_shared(QWidget *parent = nullptr);
    ~Page_shared();

    void init_table();
    void mklink(QString target, QString link);
    void start_download_status_timer();
    void On_http_server_finished();

public slots:


private:
    Ui::Page_shared *ui;
    QProcess  *p_http_server;
    bool  b_start_webserver_auto;
    bool  b_destroy;

protected:
    virtual void resizeEvent(QResizeEvent *e) override;
private slots:
    void on_shared_file_tableWidget_itemClicked(QTableWidgetItem *item);
    void on_bt_add_share_file_clicked();
    void on_bt_share_file_clicked();
    void on_bt_delete_share_clicked();
};

#endif // PAGE_SHARED_H
