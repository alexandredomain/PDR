#include "mainwindow.h"
#include <QApplication>
#include <sqlite3.h>
extern "C" {
    #include <requetesSQL.h>
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    sqlite3 * db;
    //createTableListeBatiments(db);
    //actualiserBatimentsEtSurfaces(db);

    MainWindow w;
    w.show();

    return a.exec();
}
