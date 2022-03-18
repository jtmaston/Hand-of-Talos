#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHostAddress>
#include <QDataStream>

#include <chrono>

QDataStream &operator<<(QDataStream &out, const Instruction &item)
{  
    out << item.opcode;
    for(int i = 0 ; i < 10; i++)
        out << item.params[i];
    return out;
}
QDataStream &operator>>(QDataStream &in, Instruction &item)
{
    in >> item.opcode;
    for(int i = 0 ; i < 10; i++)
        in >> item.params[i];
    return in;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    set_camera_bar_visibility(HIDDEN);
    set_learn_bar_visibility(HIDDEN);

    Scheduler_100ms = new QTimer(this); // timer called every 100ms
    Scheduler_16ms = new QTimer(this);  // timer called every  20ms

    connect(Scheduler_100ms, SIGNAL(timeout()), SLOT(update_axes()));         // axis readout is updated every 100ms
    connect(Scheduler_100ms, SIGNAL(timeout()), SLOT(command()));             // control from the axis is also updated ever 100ms
    connect(Scheduler_100ms, SIGNAL(timeout()), SLOT(check_if_filedialog())); // camera is updated every 20ms
    //connect(Scheduler_100ms, SIGNAL(timeout()), SLOT(calculate_coords()));
    

    connect(ui->learn_btn, SIGNAL(clicked()), SLOT(toggle_learn_bar()));  // when the learn button is clicked, toggle the bar
    connect(ui->track_btn, SIGNAL(clicked()), SLOT(toggle_camera_bar())); // when the track button is clicked, toggle the bar

    connect(ui->next, SIGNAL(clicked()), SLOT(add_step()));           //  <<
    connect(ui->prev, SIGNAL(clicked()), SLOT(remove_step()));        //  buttons for the learn mode
    connect(ui->execute, SIGNAL(clicked()), SLOT(follow_path()));     //  >>
    connect(ui->follow_stop, SIGNAL(clicked()), SLOT(stop_follow())); //  >>

    connect(ui->follow_red, SIGNAL(clicked()), SLOT(start_follow_red()));
    connect(ui->follow_green, SIGNAL(clicked()), SLOT(start_follow_green()));
    connect(ui->follow_blue, SIGNAL(clicked()), SLOT(start_follow_blue()));

    connect(ui->halt_btn, SIGNAL(clicked()), SLOT(halt()));
    connect(ui->jog_btn, SIGNAL(clicked()), SLOT(jog()));
    connect(ui->load_btn, SIGNAL(clicked()), SLOT(program()));

    Scheduler_100ms->start(100);
    Scheduler_16ms->start(16);

    ui->a4_r->setValue(-90);
    //running = true;

    connect(ui->base_r, &QSlider::valueChanged, ui->increment_1, &QSpinBox::setValue);
    connect(ui->a2_r, &QSlider::valueChanged, ui->increment_2, &QSpinBox::setValue);
    connect(ui->a3_r, &QSlider::valueChanged, ui->increment_3, &QSpinBox::setValue);
    connect(ui->a4_r, &QSlider::valueChanged, ui->increment_4, &QSpinBox::setValue);
    connect(ui->a5_r, &QSlider::valueChanged, ui->increment_5, &QSpinBox::setValue);
    connect(ui->grip_r, &QSlider::valueChanged, ui->increment_6, &QSpinBox::setValue);

    connect(ui->increment_1, QOverload<int>::of(&QSpinBox::valueChanged), ui->base_r, &QSlider::setValue);
    connect(ui->increment_2, QOverload<int>::of(&QSpinBox::valueChanged), ui->a2_r, &QSlider::setValue);
    connect(ui->increment_3, QOverload<int>::of(&QSpinBox::valueChanged), ui->a3_r, &QSlider::setValue);
    connect(ui->increment_4, QOverload<int>::of(&QSpinBox::valueChanged), ui->a4_r, &QSlider::setValue);
    connect(ui->increment_5, QOverload<int>::of(&QSpinBox::valueChanged), ui->a5_r, &QSlider::setValue);
    connect(ui->increment_6, QOverload<int>::of(&QSpinBox::valueChanged), ui->grip_r, &QSlider::setValue);
    sock.connectToHost(QHostAddress("127.0.0.1"), 8123);

    while(!sock.isOpen())
    {

    }
    std::cout << "Socket open";
    std::cout.flush();
    sock.write(QByteArray("Hello!\n"));

    connect(&sock, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

    go_home(); // reset the robot to the home position
}

MainWindow::~MainWindow()
{
    //running = false;
    cam_thread.waitForFinished();
    joy_thread.waitForFinished();

    
    
    //delete joystick;
    delete ui;
}
void MainWindow::toggle_learn_bar() // toggle the learning bar,
{
    set_camera_bar_visibility(HIDDEN);         // hide the camera bar
    learn_bar_state = !learn_bar_state;        // << set if the learning bar is visible or not
    set_learn_bar_visibility(learn_bar_state); // >>
    learn();                                   // and go into the learn mode
}

void MainWindow::toggle_camera_bar() // ditto for the camera bar
{
    set_learn_bar_visibility(HIDDEN);
    camera_bar_state = !camera_bar_state;
    set_camera_bar_visibility(camera_bar_state);
}

void MainWindow::set_camera_bar_visibility(bool state) // hide the camera bar, by setting height to 0
{
    camera_bar_state = state;
    ui->follow_blue->setVisible(camera_bar_state);
    ui->follow_red->setVisible(camera_bar_state);
    ui->follow_green->setVisible(camera_bar_state);
    ui->follow_stop->setVisible(camera_bar_state);
}

void MainWindow::set_learn_bar_visibility(bool state) // hide the learn bar, by setting the height to 0
{
    learn_bar_state = state;
    ui->execute->setVisible(learn_bar_state);
    ui->next->setVisible(learn_bar_state);
    ui->prev->setVisible(learn_bar_state);
}

void MainWindow::update_axes() // this updates the axes display
{
    // TODO: CALL OVER WIRE
    //float32_t *data = dev.servo_readall(); // read the values from all of the servos
    float32_t data[] = {0, 0, 0, 0, 0, 0};
    home_position = std::vector<int> {0, 0, 0, 0, 0, 0};
    //dev.angles = data;

    ui->a1_d->setText(std::string(std::string("Axis 1: ") + std::to_string(int(data[0] - home_position[0])) + "°").c_str()); // and set the strings for
    ui->a2_d->setText(std::string(std::string("Axis 2: ") + std::to_string(int(data[1] - home_position[1])) + "°").c_str()); // the labels
    ui->a3_d->setText(std::string(std::string("Axis 3: ") + std::to_string(int(data[2] - home_position[2])) + "°").c_str());
    ui->a4_d->setText(std::string(std::string("Axis 4: ") + std::to_string(int(data[3] - home_position[3])) + "°").c_str());
    ui->a5_d->setText(std::string(std::string("Axis 5: ") + std::to_string(int(data[4] - home_position[4])) + "°").c_str());
    ui->a6_d->setText(std::string(std::string("Axis 6: ") + std::to_string(int(data[5] - home_position[5])) + "°").c_str());

    float32_t end_effector [ 16 ];
    //dev.calculate_end_effector(end_effector);         TODO: CALL OVER THE WIRE
    
    ui -> DK_X -> setText( QString ( std::to_string(end_effector[12]).c_str()));
    ui -> DK_Y -> setText( QString ( std::to_string(end_effector[13]).c_str()));
    ui -> DK_Z -> setText( QString ( std::to_string(end_effector[14]).c_str()));

    //delete data; // readall returns a dynamic pointer, so it must be deleted to prevent memory leaks
}

void MainWindow::command() // get the values from the sliders, then write them on the bus
{
    int32_t angles[6];
    angles[0] = ui->base_r->value() + 90; // need to adjust with 90
    angles[1] = ui->a2_r->value() + 90;
    angles[2] = ui->a3_r->value() + 90;
    angles[3] = ui->a4_r->value() + 90;
    angles[4] = ui->a5_r->value() + 90;
    angles[5] = ui->grip_r->value() + 90;

    Instruction instr;
    instr.opcode = ANGS;
    for(int i = 0 ; i < 6; i++)
        instr.params[i] = angles[i];
    
    //QByteArray data = QByteArray((char*)&instr);
    //std::cout << data.size() << '\n';

    QDataStream out;

    out.setDevice(&sock);
    out.setVersion(QDataStream::Qt_4_0);

    out.startTransaction();
    out << instr;
    if(!out.commitTransaction())
        std::cout << "FAIL\n";
    

    //sock.write((char*)&instr, sizeof(Instruction));
    //sock.flush();
    //sock.waitForBytesWritten(3000);
    //dev.servo_write6(angles, time_mod); // move the axes      TODO: SEND OVER WIRE
}

void MainWindow::learn()
{
    // TODO: SEND OVER WIRE
}

void MainWindow::add_step()
{
    // TODO: SEND OVER WIRE
    ui->execute->setText(QString(std::to_string(ui->execute->text().toInt() + 1).c_str())); // then update the label
}

void MainWindow::remove_step()
{
    // TODO: SEND OVER WIRE
    ui->execute->setText(QString(std::to_string(ui->execute->text().toInt() - 1).c_str())); // then update the label
}

void MainWindow::follow_path()
{
    // TODO: SEND OVER WIRE
}


void MainWindow::halt()
{
    //dev.toggleTorque(false);  // TODO: FIND EMERGENCY PIN
}

void MainWindow::jog()
{
    // TODO: SEND OVER WIRE
    toggle_jog();
}

void MainWindow::update_stick()
{
    // TODO: CALL OVER WIRE
    ui->base_r->setValue(ui->base_r->value() - round(axes[0] * 100) * 0.05);
    ui->a2_r->setValue(ui->a2_r->value() - round(axes[2] * 100) * 0.05);
    ui->a3_r->setValue(ui->a3_r->value() - round(axes[1] * 100) * 0.05);
    ui->a4_r->setValue(ui->a4_r->value() - round(axes[3] * 100) * 0.05);
    ui->a5_r->setValue(ui->a5_r->value() - round(axes[4] * 100) * 0.05);
    ui->grip_r->setValue(ui->grip_r->value() - round(axes[5] * 100) * 0.1);
}

/*void MainWindow::poll_joystick()
{
    while (running)
    {
        // Restrict rate
        usleep(1000);

        // Attempt to sample an event from the joystick
        JoystickEvent event;
        if (joystick->sample(&event))
        {
            if (event.isAxis())
            {
                // std::cout << "Axis " << (int) event.number << " is at " << event.value / 32767.0f<< '\n';
                std::cout.flush();
                axes[event.number] = event.value / 32767.0f;
            }
        }
    }
    return;
}*/

void MainWindow::toggle_jog()
{
    ui->base_r->setDisabled(following_program);
    ui->increment_1->setDisabled(following_program);
    ui->a2_r->setDisabled(following_program);
    ui->increment_2->setDisabled(following_program);
    ui->a3_r->setDisabled(following_program);
    ui->increment_3->setDisabled(following_program);
    ui->a4_r->setDisabled(following_program);
    ui->increment_4->setDisabled(following_program);
    ui->a5_r->setDisabled(following_program);
    ui->increment_5->setDisabled(following_program);
    ui->grip_r->setDisabled(following_program);
    ui->increment_6->setDisabled(following_program);
}

void MainWindow::program()
{
    //following_program = !following_program;
    //toggle_jog();
    //prog_thread = QtConcurrent::run(this, &MainWindow::RASM_Interpreter);
}

void MainWindow::check_if_filedialog()
{
    if (fileopen)
    {
        filename = QFileDialog::getOpenFileName(this,
                                                "Pick the program file: ", "./", "Compiled robot Assembly Files (*.bin)");

        if (filename.size() == 0)
        {
            following_program = false;
            fileopen = false;
        }
    }
}
void MainWindow::go_home(){}


void MainWindow::stop_follow()
{

}

void MainWindow::start_follow_red()
{

}

void MainWindow::start_follow_green()
{
    
}

void MainWindow::start_follow_blue()
{
    
}
void MainWindow::onReadyRead()
{

}