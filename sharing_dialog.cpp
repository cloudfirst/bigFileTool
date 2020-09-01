#include "sharing_dialog.h"
#include "ui_sharing_dialog.h"
#include <qclipboard.h>

sharing_Dialog::sharing_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::sharing_Dialog)
{
    ui->setupUi(this);
}

sharing_Dialog::~sharing_Dialog()
{
    delete ui;
}

void sharing_Dialog::init_data(QString link, QString pass_word)
{
    m_byteArray = link;
    m_pass_word = pass_word;
    ui->shared_link->setText(m_byteArray.toStdString().c_str());
    ui->pass_word->setText(pass_word);
}

void sharing_Dialog::on_bt_copy_clicked()
{
    QClipboard *clipboard = QApplication::clipboard();
    QString originalText = QString("链接: https://bft.oxfold.cn/s/") + m_byteArray.toStdString().c_str() + " 密码: " + m_pass_word;
    clipboard->setText(originalText);
}

void sharing_Dialog::on_bt_quit_clicked()
{
    this->close();
}
