/** @file types.h
 *
 * @brief This file defines constants and main global types
 *
 */

/* includes */
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <dirent.h>
#include <time.h>
#include <pigpio.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <fluidsynth.h>

/* default GPIO pins */
#define LED_GPIO	19
#define SWITCH_GPIO	26
#define ANTIBOUNCE_US   250000      // 0.25 sec = 250000 usec : used for switch anti-bouncing check : allows 240BPM max
#define TIMEON_US       200000      // 0.20 sec : used as on/off time for leds 

/* default soundfont file */
#define DEFAULT_SF2 "./soundfonts/00_FluidR3_GM.sf2"

/* define status, etc */
#define TRUE 1
#define FALSE 0
#define OFF 0
#define ON 1

/* other defaults */
#define NB_CHANNEL	8	// 8 channel
#define NB_RECSHIFT	2	// shift key has 2 positions (non-shift & shift)
#define NB_CYCSHIFT	2	// shift key has 2 positions (non-shift & shift)

/* types */
typedef struct {				// structure for each control
	uint8_t message [3];			// midi message of the control (sent from device to PI)
	uint8_t value;					// value of the control
	void (*action) (int);		// function to be called if control is actioned
} slider_t;

typedef struct {				// structure for each control
	uint8_t message [3];			// midi message of the control (sent from device to PI)
	uint8_t value;					// value of the control
	void (*action) (int);		// function to be called if control is actioned
} knob_t;

typedef struct {				// structure for each control
	uint8_t message [3];			// midi message of the control (sent from device to PI)
	uint8_t value;					// value of the control
	void (*action) (int);		// function to be called if control is actioned
	uint8_t led_on [3];			// message to turn led on
	uint8_t ledd_off [3];			// message to turn led off
} button_t;

typedef struct {				// structure for each channel control
	slider_t slider;
	knob_t knob;
	button_t solo;
	button_t mute;
	button_t rec;				// rec is used as shift key
} channel_t;
