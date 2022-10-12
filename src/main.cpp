#include "mainwindow.hpp"

#include <QApplication>
#include <QLocale>
#include <QTranslator>



auto main(int argc, char *argv[]) -> int
{   
    char arg[] = "DISPLAY=:0.0";
    putenv(arg);
    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");
    
    QApplication a(argc, argv);
    QTranslator translator;
    const QStringList ui_languages = QLocale::system().uiLanguages();
    for (const QString &locale : ui_languages) {
        const QString base_name = "RobotDashBoard_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + base_name)) {
            QApplication::installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.show();
    return QApplication::exec();

}
