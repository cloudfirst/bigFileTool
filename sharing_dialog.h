#ifndef SHARING_DIALOG_H
#define SHARING_DIALOG_H

#include <QDialog>
#include <QString>
#include <QByteArray>

namespace Ui {
class sharing_Dialog;
}

class sharing_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit sharing_Dialog(QWidget *parent = nullptr);
    ~sharing_Dialog();

    void init_data(QString link, QString pass_word);

private slots:
    void on_bt_copy_clicked();
    void on_bt_quit_clicked();

private:
    Ui::sharing_Dialog *ui;
    QString    m_byteArray;
    QString    m_pass_word;
};

#endif // SHARING_DIALOG_H
