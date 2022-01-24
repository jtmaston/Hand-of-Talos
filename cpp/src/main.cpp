#include "lib/mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    /*QApplication a(argc, argv);
    // background-color: rgba(60.5625, 60.5625, 73.5675, 1)
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
    return a.exec();*/
    ArmDevice bot;
    //bot.ping_servo(1);
    //bot.home_position();
    bot.home_position();
    return 0;
}
