DTBO_NAME = overlays.dtbo
DTS_SOURCE = overlay.dts

all: $(DTBO_NAME)
$(DTBO_NAME): $(DTS_SOURCE)
	dtc -@ -I dts -O dtb -o $(DTBO_NAME) $(DTS_SOURCE)

clean:
	rm -f *.dtbo