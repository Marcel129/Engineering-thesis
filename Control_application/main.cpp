#include "mainwindow.h"

#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //D:\\Studia AiR\\Sem 7\\Inżynierka\\Soft\\ApkaDesktop\\TCobra.qss
    QFile styleSheetFile("D:\\Studia AiR\\Sem 7\\Inżynierka\\Soft\\ApkaDesktop\\Genetive.qss");
    styleSheetFile.open(QFile::ReadOnly);
    QString styleAheet = QLatin1String(styleSheetFile.readAll());
    a.setStyleSheet(styleAheet);

    MainWindow w;
    w.show();
    return a.exec();


}
