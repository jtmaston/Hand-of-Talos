#include "lib/mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <stdlib.h>

int main(int argc, char *argv[])
{   
    putenv("DISPLAY=:0.0");
    QApplication a(argc, argv);
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "RobotDashBoard_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.show();
    return a.exec();


    //ArmDevice bot;
}
