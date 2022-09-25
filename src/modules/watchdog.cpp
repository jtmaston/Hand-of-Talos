#include "Watchdog.hpp"

#include <iostream>

Watchdog::Watchdog(RobotArm *device, uint16_t *tme_md)
{
    dev = device;
    time_mod = tme_md;
    for (int i = 0; i < 6; i++)
    {
        destination.push_back(0);
        source.push_back(0);
    }
}

void Watchdog::register_wd()
{
    wd_active = true;
    wd_thread = QtConcurrent::run(this, &Watchdog::handler);
    std::cout << "Watchdog registered\n";
}
void Watchdog::deregister_wd()
{
    wd_active = false;
    wd_thread.waitForFinished();
    std::cout << "Watchdog deregistered\n";
}
void Watchdog::handler()
{
    while (wd_active)
    {
        //state = dev->servo_readall();

        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - started);

        float32_t factor = duration.count() / (*time_mod * 1000);

        std::cout << "We expect to be: ";
        for (int i = 0; i < 6; i++)
        {
            float32_t p = (destination.at(i) - source.at(i)) * factor;
            std::cout << p + source.at(i) << " ";
        }
        std::cout << '\n';
        std::cout << "We actually are: ";
        for (int i = 0; i < 6; i++)
        {
            std::cout << dev->angles[i] + dev->home_position.at(i) << " ";
        }
        std::cout << '\n';
        //delete state;
        usleep(10);
    }

    
}