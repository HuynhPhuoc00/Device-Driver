# -----------------------------
# User-space Application
# -----------------------------
APP_NAME = lcd_app.elf
APP_OBJS = lcd_app.o
CROSS_COMPILE = arm-linux-gnueabihf-

all: $(APP_OBJS)
	$(CROSS_COMPILE)gcc -static -o $(APP_NAME) $(APP_OBJS)

lcd_app.o: lcd_app.c
	$(CROSS_COMPILE)gcc -c -o $@ $<

clean:
	rm -f $(APP_OBJS) $(APP_NAME)