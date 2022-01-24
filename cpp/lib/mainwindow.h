#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#define HIDDEN false
#define VISIBLE true;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
   public slots:
        void toggle_learn_bar();
        void toggle_camera_bar();

private:
    Ui::MainWindow *ui;
    bool learn_bar_state = HIDDEN;
    bool camera_bar_state = HIDDEN;
    void set_learn_bar_visibility(bool state);
    void set_camera_bar_visibility(bool state);
};
#endif // MAINWINDOW_H
