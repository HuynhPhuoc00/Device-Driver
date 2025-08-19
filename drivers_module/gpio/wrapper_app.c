#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include<stdint.h>
#include <time.h>
#include <math.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

#include "dht11.h"
#include "74HC595.h"
#include "gpio.h"

int main(int argc, char *argv[])
 {
	char msg_buf[81];
	uint8_t value;

	printf("This Application Control IC 74hc595 \n");

/* For test 74HC595*/
	init_gpios();
	Write_data(value, MSB);

/* For test dht11 */
	start_dht();
	Read_DHT();

	return 0;
}


