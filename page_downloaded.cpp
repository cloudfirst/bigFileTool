#include "page_downloaded.h"
#include "ui_page_downloaded.h"

#include <QResizeEvent>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QFileInfoList>
#include <QDateTime>
#include <QProcess>
#include <QDesktopServices>

#include "mytool.h"


Page_downloaded::Page_downloaded(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Page_downloaded)
{
    ui->setupUi(this);
    this->init_table();
}

Page_downloaded::~Page_downloaded()
{
    delete ui;
}

void Page_downloaded::add_new_row(QString fname)
{
    QTableWidget * t = ui->tableWidget;
    t->clearContents();
    t->setRowCount(0);
    init_table();
}

void Page_downloaded::init_table()
{
    QTableWidget * t = ui->tableWidget;

    t->verticalHeader()->setVisible(false);
    t->setSelectionBehavior(QAbstractItemView::SelectRows);
    t->setSelectionMode( QAbstractItemView::SingleSelection );

     // init data
    int n_cols = 4;
#if defined(_WIN32)
     QDir dir1(QDir::toNativeSeparators(QDir::homePath()) + "\\oxfold\\bigfiletool\\downloaded\\");
#else
    QDir dir1(QDir::homePath() + "/oxfold/bigfiletool/downloaded/");
#endif
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
                i->setText( MyTool::converFileSizeToKBMBGB(fileInfo.size()));
            }
            if (col == 3 ) {
                i->setText("下载完成: " + fileInfo.lastModified().toString("yyyy.MM.dd hh:mm"));
            }
            t->setItem(t->rowCount()-1, col, i);
        }
    }
}

void Page_downloaded::resizeEvent(QResizeEvent *e)
{
    QTableWidget * t = ui->tableWidget;

    QPoint point = ui->tableWidget->mapToParent((ui->tableWidget->rect().topLeft()));
    QSize size = e->size();

    int w = size.width() - point.x();
    int h = size.height() - point.y();

    t->resize(w, h);

    t->setColumnWidth(0, 35);
    t->setColumnWidth(1, t->width()/2); //file name
    t->setColumnWidth(2, t->width()/6); //file size
    t->setColumnWidth(3, t->width()/3); //progress bar

    QSize  lsize = ui->label->size();
    ui->label->resize(size.width(), lsize.height());

}

void Page_downloaded::on_bt_show_directory_clicked()
{
#ifdef Q_OS_MAC
    QProcess::execute("open", QStringList() << QDir::homePath() + "/oxfold/bigfiletool/downloaded/");
#endif

#ifdef Q_OS_WIN32
    QString arg = QDir::toNativeSeparators(QDir::homePath()) + "\\oxfold\\bigfiletool\\downloaded";
    QProcess::execute("explore.exe", QStringList() << arg);
#endif
}
