#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QThread>
#include "databasehandler.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
