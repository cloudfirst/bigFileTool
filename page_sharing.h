#ifndef PAGE_SHARING_H
#define PAGE_SHARING_H

#include <QWidget>

namespace Ui {
class Page_sharing;
}

class Page_sharing : public QWidget
{
    Q_OBJECT

public:
    explicit Page_sharing(QWidget *parent = nullptr);
    ~Page_sharing();

private:
    Ui::Page_sharing *ui;
};

#endif // PAGE_SHARING_H
