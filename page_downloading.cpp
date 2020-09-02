#include "page_downloading.h"
#include "ui_page_downloading.h"
#include <QResizeEvent>
#include <QFileDialog>
#include <QDateTime>
#include "oxfold_wrapper.h"
#include <QDebug>
#include "add_link_dialog.h"

Page_downloading::Page_downloading(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Page_downloading)
{
    ui->setupUi(this);
    init_table();
}

Page_downloading::~Page_downloading()
{
    delete ui;
}

void Page_downloading::init_table()
{
    QTableWidget * t = ui->tableWidget;

    t->verticalHeader()->setVisible(false);
    t->setSelectionBehavior(QAbstractItemView::SelectRows);
    t->setSelectionMode( QAbstractItemView::SingleSelection );

    // init data
    int n_cols = 4;
    QDir dir1(QDir::homePath() + "/oxfold/bigfiletool/downloading");
    QFileInfoList list = dir1.entryInfoList();
    for (int i = 0; i < list.size(); ++i)
    {
        QFileInfo fileInfo = list.at(i);
        if (fileInfo.isDir())
            continue;

        t->insertRow(t->rowCount());

        for (int col=0; col!=n_cols; ++col)
        {
            QTableWidgetItem * const i = new QTableWidgetItem;
            i->setFlags(i->flags() & ~Qt::ItemIsEditable);
            if (col == 0 ) {
                //Checkbox
                i->setFlags(
                    Qt::ItemIsSelectable
                  | Qt::ItemIsUserCheckable
                );
                i->setCheckState(Qt::Unchecked);
            }
            if (col == 1 ) {
                i->setText(fileInfo.fileName());
            }
            if (col == 2 ) {
                i->setText(converFileSizeToKBMBGB(fileInfo.size()));
            }
            if (col == 3 ) {
                i->setText("下载中... ...");
            }
            t->setItem(t->rowCount()-1, col, i);
        }
    }
}

void Page_downloading::resizeEvent(QResizeEvent *e)
{
    QTableWidget * t = ui->tableWidget;

    QPoint point = ui->tableWidget->mapToParent((ui->tableWidget->rect().topLeft()));
    QSize size = e->size();

    int w = size.width() - point.x();
    int h = size.height() - point.y();

    t->resize(w, h);

    t->setColumnWidth(0, 35);
    t->setColumnWidth(1, t->width()/2);
    t->setColumnWidth(2, t->width()/6);
    t->setColumnWidth(3, t->width()/3);

    QSize  lsize = ui->label->size();
    ui->label->resize(size.width(), lsize.height());

}

void Page_downloading::on_bt_pause_all_clicked()
{

}

void Page_downloading::on_bt_start_all_clicked()
{

}

void Page_downloading::on_bt_delete_clicked()
{

}

void Page_downloading::on_bt_add_clicked()
{
    Add_link_Dialog dlg(this);
    dlg.exec();
}
