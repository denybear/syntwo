/** @file gpio.c
 *
 * @brief This is the file driven gpio / tap-tempo / beat switch related functions.
 *
 */

#include "types.h"
#include "globals.h"
#include "config.h"
#include "process.h"
#include "utils.h"
#include "gpio.h"


// you need to have root priviledges for it to work
// do not use, use gpio deamon instead
/*
int init_gpio ()
{
	if (gpioInitialise() == PI_INIT_FAILED) {
    	fprintf(stderr, "pigpio initialisation failed\n");
    	return OFF;
	}

	// Set GPIO modes
	gpioSetMode(LED_GPIO, PI_OUTPUT);
	gpioWrite (LED_GPIO, OFF);					// at start, LED is off

	gpioSetMode(SWITCH_GPIO, PI_INPUT);
	gpioSetPullUpDown(SWITCH_GPIO, PI_PUD_UP);	// Sets a pull-up
	// benefits of pull-up is that way, no voltage are input in the pins; pins are only put to GND
	return ON;
}
*/

int init_gpio ()
{
	gpio_deamon = pigpio_start(0,0);		// connect to localhost on port 8888

	if (gpio_deamon < 0) {
    	fprintf(stderr, "pigpio initialisation failed\n");
    	return OFF;
	}

	/* Set GPIO modes */
	set_mode (gpio_deamon, LED_GPIO, PI_OUTPUT);
	gpio_write (gpio_deamon, LED_GPIO, OFF);					// at start, LED is off

	set_mode (gpio_deamon, SWITCH_GPIO, PI_INPUT);
	set_pull_up_down (gpio_deamon, SWITCH_GPIO, PI_PUD_UP);	// Sets a pull-up
	// benefits of pull-up is that way, no voltage are input in the pins; pins are only put to GND
	return ON;
}


// you need to have root priviledges for it to work
// do not use, use gpio deamon instead
/*
int kill_gpio ()
{
	gpioTerminate ();
}
*/

int kill_gpio ()
{
	gpio_state = OFF;
	pigpio_stop (gpio_deamon);
}


// process managing external switch and LED
int gpio_process () {

	// get current time
	now = micros ();

	// test if GPIO is enabled
	if (gpio_state == ON) {

		// test switch value; if switch is pressed then value is LOW
//		if (gpioRead (SWITCH_GPIO) == OFF) {
		if (gpio_read (gpio_deamon, SWITCH_GPIO) == OFF) {
			// anti_bounce mechanism: make sure the switch is not "bouncing", causing repeated ON-OFF in a short period
			// no bounce if previous is 0
			if ((previous == 0) || ((now-previous) >= ANTIBOUNCE_US))
			{
				previous_led = now;			// set time when led has been put on
//				gpioWrite (LED_GPIO, ON);	// turn LED ON
				gpio_write (gpio_deamon, LED_GPIO, ON);	// turn LED ON
				return TRUE;				// switch pressed, no bounce : exit function with press	OK
			}
		}

		// check when to turn LED off : it is turned off when led is on for more than TIMEON_US
	    if ((now - previous_led) > TIMEON_US) {
//			gpioWrite (LED_GPIO, OFF);	// turn LED OFF
			gpio_write (gpio_deamon, LED_GPIO, OFF);	// turn LED OFF
		}
	}

	return FALSE;		// return false (no switch press) when: 1- no GPIO enabled; 2- switch pressed but this is a bounce
}


// process callback called to process press on "beat" pad/switch 
int beat_process () {

	uint64_t tempo_us;						// for beat management

	// get current time
	now = micros ();

	tempo_us = fluid_player_get_midi_tempo (player);	// get tempo per quarter note

	// proceed only if we have a valid tempo; otherwise do nothing
	if (tempo_us != FLUID_FAILED) {

		// check if previous time is set; if not, set to a default value corresponding to current BPM
		// fluid_player_get_midi_tempo () returns current tempo of player in us per quarter note (ie per beat)
		if (previous == 0) {
			// we are here when this is the first time we press the beat button
			// take advantage to note the initial BPM of the file, just in case
			if (initial_bpm == -1) {
				initial_bpm = (fluid_player_get_bpm (player) == FLUID_FAILED) ? 0 : fluid_player_get_bpm (player);
			}

			previous = now - tempo_us;						// set value of previous according to tempo
		} 

		// check that time between previous and now is not too long...
		// in some case, we miss a beat keypress, we must accomodate for this
		// to check this, we make sure that time difference is < than 1.75 x current tempo
		// if it is >, then likely a keypress has been mised: do not change tempo then
		if ((now-previous) > tempo_us + (tempo_us >> 1) + (tempo_us >>2)) {
			previous = 0;
		}
		else {
			// set new tempo
			fluid_player_set_tempo (player, FLUID_PLAYER_TEMPO_EXTERNAL_MIDI, (double)(now-previous));
			previous = now;
		}
	}
}

