#include "page_sharing.h"
#include "ui_page_sharing.h"

Page_sharing::Page_sharing(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Page_sharing)
{
    ui->setupUi(this);
}

Page_sharing::~Page_sharing()
{
    delete ui;
}
