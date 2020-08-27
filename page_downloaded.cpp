#include "page_downloaded.h"
#include "ui_page_downloaded.h"

Page_downloaded::Page_downloaded(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Page_downloaded)
{
    ui->setupUi(this);
}

Page_downloaded::~Page_downloaded()
{
    delete ui;
}
