#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    FILE *fp;

    if (argc < 2) {
        printf("Usage: %s <text>\n", argv[0]);
        return -1;
    }

    fp = fopen("/sys/class/lcd1602/lcd1602/text", "w");
    if (!fp) {
        perror("open /sys/class/lcd1602/lcd1602/text");
        return -1;
    }

    fprintf(fp, "%s", argv);
    fflush(fp);
    fclose(fp);

    return 0;
}
