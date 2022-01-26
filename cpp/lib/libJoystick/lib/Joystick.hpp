#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/joystick.h>


class Joystick
{   
    public:
        
    private:
        int device = open(device, O_RDONLY);

        int read_event(int fd, struct js_event *event);

        struct axis_state {
            short x, y;
        };
};