#ifndef MYSHAREDFILESTABLE_H
#define MYSHAREDFILESTABLE_H
#include <QTableWidget>

class MySharedFilesTable: public QTableWidget
{
    Q_OBJECT
public:
    MySharedFilesTable();
    void init_data();
};

#endif // MYSHAREDFILESTABLE_H
