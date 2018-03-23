#include <QStyleFactory>
#include <QApplication>
#include <QTextCodec>
#include <QFile>

#include "mainwindow.h"
#include "settings.h"

int main(int argc, char *argv[]) {
    //QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    QApplication a(argc, argv);
//    QString sStyle = Settings::S("GUI", "dark");
//    QApplication::setStyle(QStyleFactory::create("plastique"));

//    if (sStyle == "dark") {
//        QString sFile;
//        if (sStyle == "dark") sFile = ":/styles/dark.qss";
//        QFile file(sFile);
//        file.open(QFile::ReadOnly);
//        QString styleSheet = QLatin1String(file.readAll());
//        a.setStyleSheet(styleSheet);
//    }
    MainWindow w;
    w.show();
    return a.exec();
}
