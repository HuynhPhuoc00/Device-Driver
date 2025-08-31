#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd;
    const char *msg;

    fd = open("/sys/bus/i2c/devices/2-0027/lcd_class/lcd1602/set_xy", O_WRONLY);
    msg = "0 0";
    write(fd, msg, strlen(msg));
    close(fd);

    fd = open("/sys/bus/i2c/devices/2-0027/lcd_class/lcd1602/text", O_WRONLY);
    write(fd, "Hello", strlen("Hello"));
    close(fd);

    fd = open("/sys/bus/i2c/devices/2-0027/lcd_class/lcd1602/set_xy", O_WRONLY);
    msg = "1 0";
    write(fd, msg, strlen(msg));
    close(fd);


    fd = open("/sys/bus/i2c/devices/2-0027/lcd_class/lcd1602/text", O_WRONLY);
    write(fd, "World", strlen("World"));
    close(fd);

    return 0;
}
