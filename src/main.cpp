#include "inc/mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <cstdlib>

// << https://gist.github.com/AndreLouisCaron/1841061 
namespace {

    // redirect outputs to another output stream.
    class RedirectOutputs
    {
        std::ostream& myStream_;
        std::streambuf *const myBuffer_;
    public:
        explicit RedirectOutputs (std::ostream& lhs, std::ostream& rhs=std::cout )
            : myStream_(rhs), myBuffer_(myStream_.rdbuf())
        {
            myStream_.rdbuf(lhs.rdbuf());
        }

        ~RedirectOutputs () {
            myStream_.rdbuf(myBuffer_);
        }
    };

    // redirect output stream to a string.
    class CaptureOutputs
    {
        std::ostringstream myContents_;
        const RedirectOutputs myRedirect_;
    public:
        explicit CaptureOutputs (std::ostream& stream=std::cout )
            : myContents_(), myRedirect_(myContents_, stream)
        {}
        auto contents () const -> std::string
        {
            return (myContents_.str());
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
    const RedirectOutputs _(log_file, std::cerr);
// >>
    #ifdef __x86_64__
        // do x64 stuff
    #else
        char arg[] = "DISPLAY=:0.0";
        putenv(arg);
    #endif
    
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
    
    return 0;
}
