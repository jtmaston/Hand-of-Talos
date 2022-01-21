#include <QApplication>
#include <QtCore>
#include <QtGui>
#include <QPushButton>
#include <QGridLayout>
#include <QSlider>

#include <QtMultimedia>
#include <QtMultimediaWidgets>

#include <QCamera>

#include <iostream>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    // Create a widget
    QWidget *w = new QWidget();

    // Window title
    w->setWindowTitle("Dashboard");
    w->setGeometry(0, 0, 1280, 720);
    QPalette pal = QPalette();

    pal.setColor(QPalette::Window, QColor(255 * 0.2375, 255 * 0.2375, 255 * 0.2885));
    w->setAutoFillBackground(true);
    w->setPalette(pal); 
    

    QGridLayout *LargeLayout = new QGridLayout;
    QPushButton *LearnButton = new QPushButton;
    QPushButton *RecogButton = new QPushButton;
    QPushButton *FllowButton = new QPushButton;
    QPushButton *StopButton  = new QPushButton;
    

    LearnButton -> setText("Mod invatare");
    RecogButton -> setText("Recunoaste obiect");
    FllowButton -> setText("Urmareste obiect");
    StopButton  -> setText("STOP");
    


    LargeLayout -> addWidget(LearnButton, 0, 0, 1, 1);
    LargeLayout -> addWidget(RecogButton, 0, 1, 1, 1);
    LargeLayout -> addWidget(FllowButton, 0, 2, 1, 1);
    LargeLayout -> addWidget(StopButton,  0, 3, 1, 1);
    
    QSlider* axes[ 6 ];
    for(int i = 1 ; i < 7; i++)
    { 
      axes[i] = new QSlider();
      axes[i] -> setOrientation(Qt::Orientation::Horizontal);
      LargeLayout -> addWidget(axes[i], i, 0, 1, 1);
    }


  
    QCamera* camera = new QCamera;
    QCameraViewfinder* viewfinder = new QCameraViewfinder;
    camera->setViewfinder(viewfinder);
    viewfinder ->
    //viewfinder->show();
    LargeLayout -> addWidget(viewfinder, 2, 1, 4, 3);

    camera->start(); // to start the viewfinder



    /*LargeLayout -> addWidget(TopBar, 0, 0, 1, 1);
    LargeLayout -> addWidget(MiddleBar, 1, 0, 1, 1);
    LargeLayout -> addWidget(BottomBar)*/

    w -> setLayout(LargeLayout);
   


    w->show(); 
    return app.exec();
}