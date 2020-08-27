#ifndef PAGE_DOWNLOADING_H
#define PAGE_DOWNLOADING_H

#include <QWidget>

namespace Ui {
class Page_downloading;
}

class Page_downloading : public QWidget
{
    Q_OBJECT

public:
    explicit Page_downloading(QWidget *parent = nullptr);
    ~Page_downloading();

private:
    Ui::Page_downloading *ui;
};

#endif // PAGE_DOWNLOADING_H
