#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
//#include <unistd.h>
//#include <fcntl.h>
//#include <ctype.h>
//#include <math.h>
//#include <errno.h>


volatile sig_atomic_t abort_signaled = 0;

#define DMX_CHANNELS 16
typedef struct /*XLASER_CFG*/ {
	uint16_t dmx_address;
	uint8_t dmx_channels[DMX_CHANNELS];
	uint8_t art_net;
	uint8_t art_subUni;
	int device;
	char* bindhost;
	char* window_name;
	bool double_buffer;
	int sockfd;
	//char* backgnd_image;
	//char** gobos;
	//chanmap?
} CONFIG;


// CHANNELS
// X 16bit
// Y 16bit
// R G B 24bit
// Dimmer
// Shutter
// Gobo
// Focus
// Rotation Abs
// Rotation Speed
#include "easy_config.h"
#include "easy_config.c"
#include "network.h"
#include "dmx4all.c"
#include "artnet.h"
#include "artnet.c"
#include "coreloop.c"
