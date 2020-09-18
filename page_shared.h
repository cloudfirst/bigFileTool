#ifndef PAGE_SHARED_H
#define PAGE_SHARED_H

#include <QWidget>
#include <QTableWidgetItem>
#include <QProcess>

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

public slots:
    void rightMessage();

private:
    Ui::Page_shared *ui;
    QProcess  *p_http_server;
    bool  b_start_webserver_auto;

protected:
    virtual void resizeEvent(QResizeEvent *e) override;
private slots:
    void on_shared_file_tableWidget_itemClicked(QTableWidgetItem *item);
    void on_bt_add_share_file_clicked();
    void on_bt_share_file_clicked();
};

#endif // PAGE_SHARED_H
