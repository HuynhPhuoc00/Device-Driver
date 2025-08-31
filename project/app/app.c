#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

void delay(int seconds) {
    struct timespec ts;
    ts.tv_sec = seconds;
    ts.tv_nsec = 0;
    nanosleep(&ts, NULL);
}


int main() {
    int fd;
    const char *msg;

    // for lcd i2c
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

    // for lcd spi
    int fb = open("/dev/fb0", O_RDWR);
    if (fb < 0) {
        perror("open fb");
        return -1;
    }

    struct fb_var_screeninfo vinfo;
    if (ioctl(fb, FBIOGET_VSCREENINFO, &vinfo)) {
        perror("ioctl FBIOGET_VSCREENINFO");
        close(fb);
        return -1;
    }

    long screensize = vinfo.yres_virtual * vinfo.xres_virtual * vinfo.bits_per_pixel / 8;
    uint16_t *fbmem = (uint16_t*) mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb, 0);

    if ((intptr_t)fbmem == -1) {
        perror("mmap");
        close(fb);
        return -1;
    }

    int width  = vinfo.xres;   // 320
    int height = vinfo.yres;   // 240

    // 6 màu RGB565
    uint16_t colors[6] = {
        0xF800, // Red
        0x07E0, // Green
        0x001F, // Blue
        0xFFFF, // White
        0xFFE0, // Yellow
        0xF81F  // Magenta
    };

    int rect_w = width / 2;   // 160
    int rect_h = height / 3;  // 80

    int idx = 0;
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 2; col++) {
            uint16_t color = colors[idx++];
            int x_start = col * rect_w;
            int y_start = row * rect_h;

            for (int y = y_start; y < y_start + rect_h; y++) {
                for (int x = x_start; x < x_start + rect_w; x++) {
                    fbmem[y * width + x] = color;
                }
            }
        }
    }

    munmap(fbmem, screensize);
    close(fb);

    return 0;
}