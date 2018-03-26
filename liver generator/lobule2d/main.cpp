#include <QStyleFactory>
#include <QApplication>
#include <QTextCodec>
#include <QFile>

#include "mainwindow.h"
#include "settings.h"

int main(int argc, char *argv[]) {

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
