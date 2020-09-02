#ifndef ADD_LINK_DIALOG_H
#define ADD_LINK_DIALOG_H

#include <QDialog>

namespace Ui {
class Add_link_Dialog;
}

class Add_link_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Add_link_Dialog(QWidget *parent = nullptr);
    ~Add_link_Dialog();

signals:
    void update_parent(QString data);

private slots:
    void on_bt_add_clicked();
    void on_bt_quit_clicked();

private:
    void try_to_get_link_from_clipboard();
    void parse_and_save_link();

private:
    Ui::Add_link_Dialog *ui;
    QString    m_byteArray;
    QString    m_pass_word;
};

#endif // ADD_LINK_DIALOG_H
