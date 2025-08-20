#include <stdio.h>
#include <stdlib.h>

#define I2C_CLASS "i2c_bus_class"
#define LCD1602_NAME "lcd1602"

int main(int argc, char *argv[])
{
    FILE *fp;

    if (argc < 2) {
        printf("Usage: %s <text>\n", argv[0]);
        return -1;
    }

    fp = fopen("/sys/class/i2c_bus_class/lcd1602/text", "w");
    if (!fp) {
        perror("open /sys/class/i2c_bus_class/lcd1602/text");
        return -1;
    }

    fprintf(fp, "%s", argv[1]);
    fflush(fp);
    fclose(fp);

    return 0;
}
