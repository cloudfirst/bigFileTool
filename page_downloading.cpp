#include "page_downloading.h"
#include "ui_page_downloading.h"

Page_downloading::Page_downloading(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Page_downloading)
{
    ui->setupUi(this);
}

Page_downloading::~Page_downloading()
{
    delete ui;
}
