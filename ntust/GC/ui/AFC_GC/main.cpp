#include "mainwindow.h"

#include <QApplication>
#include "dllSysTest.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w; 
    w.setWindowTitle("AFC_GC");
    w.show();
    return a.exec();
}
