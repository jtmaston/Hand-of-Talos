#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <stdlib.h>

// << https://gist.github.com/AndreLouisCaron/1841061 
namespace {

    // redirect outputs to another output stream.
    class redirect_outputs
    {
        std::ostream& myStream;
        std::streambuf *const myBuffer;
    public:
        redirect_outputs ( std::ostream& lhs, std::ostream& rhs=std::cout )
            : myStream(rhs), myBuffer(myStream.rdbuf())
        {
            myStream.rdbuf(lhs.rdbuf());
        }

        ~redirect_outputs () {
            myStream.rdbuf(myBuffer);
        }
    };

    // redirect output stream to a string.
    class capture_outputs
    {
        std::ostringstream myContents;
        const redirect_outputs myRedirect;
    public:
        capture_outputs ( std::ostream& stream=std::cout )
            : myContents(), myRedirect(myContents, stream)
        {}
        std::string contents () const
        {
            return (myContents.str());
        }
    };

}


int main(int argc, char *argv[])
{   
    std::ofstream log_file("/dev/null", std::ios::binary);                      // a hotfix to avoid v4l2's garbage 
    if (!log_file.is_open()) {
        std::cerr
            << "Could not open log file for writing."
            << std::endl;
        return (EXIT_FAILURE);
    }
    const redirect_outputs _(log_file, std::cerr);
// >>
    char arg[] = "DISPLAY=:0.0";
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
