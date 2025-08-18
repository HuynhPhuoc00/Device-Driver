#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    FILE *fp;

    if (argc < 2) {
        printf("Usage: %s <text>\n", argv[0]);
        return -1;
    }

    // Đường dẫn sysfs file text, trùng với driver sysfs
    fp = fopen("/sys/class/lcd1602/lcd1602/text", "w");
    if (!fp) {
        perror("open /sys/class/lcd1602/lcd1602/text");
        return -1;
    }

    // Ghi chuỗi vào sysfs → sẽ gọi init_store hoặc tương tự trong driver
    fprintf(fp, "%s", argv[1]);
    fflush(fp);  // đảm bảo ghi ra luôn
    fclose(fp);

    return 0;
}
