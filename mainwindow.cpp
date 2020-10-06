#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageLogger>
#include "page_shared.h"
#include "page_sharing.h"
#include "page_downloaded.h"
#include "page_downloading.h"
#include <QResizeEvent>
#include <QDesktopServices>

#include "mytool.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    MyTool::killAllProcess();

    ui->setupUi(this);
    //ui->shareList->setCurrentRow(0);
    ui->download_list->setCurrentRow(0);

    //start oxfold network service
    MyTool::init_bft_env();

    Page_shared * page1 = new Page_shared();
    ui->stackedWidget->addWidget(page1);

    Page_sharing * page2 = new Page_sharing();
    ui->stackedWidget->addWidget(page2);

    Page_downloading * page3 = new Page_downloading();
    ui->stackedWidget->addWidget(page3);

    Page_downloaded * page4 = new Page_downloaded();
    ui->stackedWidget->addWidget(page4);

    connect(page3, SIGNAL(download_task_finished(QString)), page4, SLOT(add_new_row(QString)));

    ui->label_website->setOpenExternalLinks(true);
    ui->label_website->setText("<a href=\"http://www.oxfold.cn\">http://www.oxfold.cn</a>");
}

MainWindow::~MainWindow()
{
    delete ui;
    MyTool::close_bft_env();

    MyTool::killAllProcess();
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
    if (item->text() == "本地文件") {
        ui->stackedWidget->setCurrentIndex(0);
    }
    if (item->text() == "正在下载") {
        ui->stackedWidget->setCurrentIndex(2);
    }
    if (item->text() == "下载完成") {
        ui->stackedWidget->setCurrentIndex(3);
    }
}
