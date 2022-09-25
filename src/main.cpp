#include "mainwindow.hpp"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <stdlib.h>



int main(int argc, char *argv[])
{   
    //char arg[] = "DISPLAY=:0.0";
    //putenv(arg);
    //qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");
    
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
    
    return 0;
}
