#include "mainwindow.h"

#include <QApplication>
#include <QDir>

void init_bft_env()
{
    //create directories at home dir
    // ~/oxfold/bigfiletool/shared
    // ~/oxfold/bigfiletool/downloaded
    // ~/oxfold/bigfiletool/downloading
    QDir dir1(QDir::homePath() + "/oxfold/bigfiletool/shared");
    if (!dir1.exists())
    {
        dir1.mkpath(QDir::homePath() + "/oxfold/bigfiletool/shared");
    }
    QDir dir2(QDir::homePath() + "/oxfold/bigfiletool/downloaded");
    if (!dir2.exists())
    {
        dir2.mkpath(QDir::homePath() + "/oxfold/bigfiletool/downloaded");
    }
    QDir dir3(QDir::homePath() + "/oxfold/bigfiletool/downloading");
    if (!dir3.exists())
    {
        dir3.mkpath(QDir::homePath() + "/oxfold/bigfiletool/downloading");
    }

    // and init locale DB

}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    init_bft_env();
    return a.exec();
}
