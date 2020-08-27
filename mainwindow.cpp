#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageLogger>
#include "page_shared.h"
#include "page_sharing.h"
#include "page_downloaded.h"
#include "page_downloading.h"
#include <QResizeEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->shareList->setCurrentRow(0);

    QWidget * page1 = new Page_shared();
    ui->stackedWidget->addWidget(page1);

    QWidget * page2 = new Page_sharing();
    ui->stackedWidget->addWidget(page2);

    QWidget * page3 = new Page_downloading();
    ui->stackedWidget->addWidget(page3);

    QWidget * page4 = new Page_downloaded();
    ui->stackedWidget->addWidget(page4);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    QWidget * qw = ui->stackedWidget;

    QPoint point = ui->stackedWidget->mapToParent((ui->stackedWidget->rect().topLeft()));
    QSize size = e->size();

    int w = size.width() - point.x();
    int h = size.height() - point.y();

    qw->resize(w, h);
}

void MainWindow::on_shareList_itemClicked(QListWidgetItem *item)
{
    if (item->text() == "共享文件") {
        ui->stackedWidget->setCurrentIndex(0);
    }
    if (item->text() == "正在共享") {
        ui->stackedWidget->setCurrentIndex(1);
    }
}

void MainWindow::on_download_list_itemClicked(QListWidgetItem *item)
{
    if (item->text() == "正在下载") {
        ui->stackedWidget->setCurrentIndex(2);
    }
    if (item->text() == "下载完成") {
        ui->stackedWidget->setCurrentIndex(3);
    }
}
