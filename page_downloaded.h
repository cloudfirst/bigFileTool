#ifndef PAGE_DOWNLOADED_H
#define PAGE_DOWNLOADED_H

#include <QWidget>

namespace Ui {
class Page_downloaded;
}

class Page_downloaded : public QWidget
{
    Q_OBJECT

public:
    explicit Page_downloaded(QWidget *parent = nullptr);
    ~Page_downloaded();

private:
    Ui::Page_downloaded *ui;
};

#endif // PAGE_DOWNLOADED_H
