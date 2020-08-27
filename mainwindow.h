#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_shareList_itemClicked(QListWidgetItem *item);

    void on_download_list_itemClicked(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;

protected:
    virtual void resizeEvent(QResizeEvent *e) override;
};
#endif // MAINWINDOW_H
