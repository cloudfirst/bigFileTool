#ifndef PAGE_DOWNLOADING_H
#define PAGE_DOWNLOADING_H

#include <QWidget>
#include <QTableWidgetItem>

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

private slots:
    void on_bt_pause_all_clicked();

    void on_bt_start_all_clicked();

    void on_bt_delete_clicked();

    void on_bt_add_clicked();

private:
    Ui::Page_downloading *ui;
};

#endif // PAGE_DOWNLOADING_H
