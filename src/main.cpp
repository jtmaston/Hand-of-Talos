#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <stdlib.h>

int main(int argc, char *argv[])
{   
    char arg[] = "DISPLAY=:0.0";
    putenv(arg);
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

    //ArmDevice bot;
   // bot.servo_write(2, (float)180, 2000);

    //ArmDevice bot;
    //bot.servo_write(1, (float) 10, 10000);

    // axis 0 servo: 25  -> 335
    // axis 1 servo: 100 -> 300

    //bot.servo_write(0, (float)90.0, 1000);

    // 1 : seems ok | 2 : seems ok | 3 : 94 | 4 : 95 | 5 : seems ok
    //ArmDevice bot;
}
