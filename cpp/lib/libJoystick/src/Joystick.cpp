#include "../lib/Joystick.hpp"

/*int Joystick::read_event(int fd, struct js_event *event)
{
    //uint32_t data = read(fd, event, sizeof(*event));
}*/
void Joystick::get_axis_state(js_event* event)
{
    size_t axis = event->number / 2;

    if (axis < 3)
    {
        if (event->number % 2 == 0)
            axes[axis].x = event->value;
        else
            axes[axis].y = event->value;
    }

}


Joystick::Joystick()
{
    device = open("/dev/input/js0", O_RDONLY);
    if ( device < 0 )
        std::cout << "Death";
}

int Joystick::read_event(struct js_event *event)
{
    ssize_t bytes;

    bytes = read(device, event, sizeof(*event));

    if (bytes == sizeof(*event))
        return 0;

    /* Error, could not read full event. */
    return -1;
}

void Joystick::Update_controllers()
{
    while (read_event(device, &event) == 0)
    {
        switch (event.type)
        {
            case JS_EVENT_BUTTON:
                printf("Button %u %s\n", event.number, event.value ? "pressed" : "released");
                break;
            case JS_EVENT_AXIS:
                axis = get_axis_state(&event, axes);
                if (axis < 3)
                    printf("Axis %zu at (%6d, %6d)\n", axis, axes[axis].x, axes[axis].y);
                break;
            default:
                /* Ignore init events. */
                break;
        }
        
        fflush(stdout);
    }
}