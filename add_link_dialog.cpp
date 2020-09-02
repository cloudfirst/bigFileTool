#include "add_link_dialog.h"
#include "ui_add_link_dialog.h"
#include <QClipboard>
#include <QtDebug>
#include "encrypt/simple_encrypt.h"
#include <QJsonDocument>
#include <QJsonObject>

Add_link_Dialog::Add_link_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Add_link_Dialog)
{
    ui->setupUi(this);
    this->try_to_get_link_from_clipboard();
}

QString simple_decrypte(QString input, QString pass_word)
{
    quint64 key = pass_word.toULongLong();
    SimpleCrypt processSimpleCrypt(key);

    QString output = processSimpleCrypt.decryptToString(input);
    return output;
}

void Add_link_Dialog::parse_link()
{
    QString json_string = simple_decrypte(m_byteArray, m_pass_word);
    QJsonObject obj;
    QJsonDocument doc = QJsonDocument::fromJson(json_string.toUtf8());
    if (!doc.isNull() && doc.isObject())
    {
        obj = doc.object();
    }
}

void Add_link_Dialog::try_to_get_link_from_clipboard()
{
    QClipboard *clipboard = QApplication::clipboard();
    QString text = clipboard->text().trimmed();
    if (text.length() > 0) {
        QStringList list = text.split("上海折叠网络荣誉出品");
        if (list.length() == 2) {
            QStringList link_list = list[0].split(" ");
            for (int index=0; index<link_list.length(); index++)
            {
                qDebug("%s", link_list[index].toStdString().c_str());
            }

            if (link_list[0] == "链接:" && link_list[2] == "密码:") {
                QString link = link_list[1];
                QString pass_word = link_list[3].trimmed();
                qDebug("链接:%s", link.toStdString().c_str());
                qDebug("密码:%s", pass_word.toStdString().c_str());
                ui->shared_link->setText(link);
                this->m_byteArray = link;
                ui->pass_word->setText(pass_word);
                this->m_pass_word = pass_word;
            }
        }
    }
}

Add_link_Dialog::~Add_link_Dialog()
{
    delete ui;
}

void Add_link_Dialog::on_bt_add_clicked()
{
    parse_link();
}

void Add_link_Dialog::on_bt_quit_clicked()
{
    this->close();
}
