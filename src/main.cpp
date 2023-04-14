#include "inc/mainwindow.hpp"

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

#include <QLabel>
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
    /*#ifdef __x86_64__
        // do x64 stuff
    #else
        char arg[] = "DISPLAY=:0.0";
        putenv(arg);
    #endif*/

    char arg[] = "DISPLAY=:0.0";
    putenv(arg);
    int retcode = 0;

    do {

        auto a = new QApplication(argc, argv);
        MainWindow w;
        w.show();
        if(retcode)
            w.changeView(retcode);
        retcode = a->exec();
        QMetaObject::invokeMethod(&w, "close", Qt::QueuedConnection);


    }while(retcode != 0);

    return 0;
}
