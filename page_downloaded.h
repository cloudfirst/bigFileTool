#ifndef PAGE_DOWNLOADED_H
#define PAGE_DOWNLOADED_H

#include <QWidget>
#include <QTableWidgetItem>

namespace Ui {
class Page_downloaded;
}

class Page_downloaded : public QWidget
{
    Q_OBJECT

public:
    explicit Page_downloaded(QWidget *parent = nullptr);
    ~Page_downloaded();

    void init_table();

public slots:
    void add_new_row(QString fname);

protected:
    virtual void resizeEvent(QResizeEvent *e) override;



private:
    Ui::Page_downloaded *ui;
};

#endif // PAGE_DOWNLOADED_H
