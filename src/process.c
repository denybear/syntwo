/** @file process.c
 *
 * @brief The process callback for this application is called when midi messages are 
 *
 */

#include "types.h"
#include "globals.h"
#include "config.h"
#include "process.h"


// fluid callback called every time a MIDI message is received
int handle_midi_event(void* data, fluid_midi_event_t* event)
{
	fluid_midi_router_t* router;


	// define data as being a pointer to fluid_midi_router_t router
	router = (fluid_midi_router_t*) data;

    printf("event type (without channel): %x %x %x\n", fluid_midi_event_get_type(event), fluid_midi_event_get_value(event), fluid_midi_event_get_velocity(event));

	return FLUID_OK;
}


// main process callback called at capture of (nframes) frames/samples
int process ( jack_nframes_t nframes, void *arg )
{
	jack_nframes_t h;
	int i,j,k;
	int mute = OFF;
	void *midiin;
	void *midiout;
	void *clockout;
	jack_midi_event_t in_event;
	jack_midi_data_t buffer[5];				// midi out buffer for lighting the pad leds and for midi clock
	int dest, row, col, on_off;				// variables used to manage lighting of the pad leds


	/*****************************************************************************/
	/* At very first, process external beat switch by checking if pressed or not */
	/*****************************************************************************/
	if (gpio_process () == TRUE) beat_process ();


	return 0;
}


// process callback called to process midi_in events in realtime
int midi_in_process (jack_midi_event_t *event, jack_nframes_t nframes) {

	int i,j;
	int k,l;


	// check if play pad has been pressed
	if (same_event(event->buffer,filename[0].ctrl[PLAY])) {
		// toggle is_play value from ON to OFF (TRUE to FALSE)
		is_play = (is_play == TRUE) ? FALSE : TRUE;

		// test if playing shall be started or stopped
		if (is_play) {
			// init clock sending to indicate PLAY has been pressed
			send_clock = CLOCK_PLAY_READY;
			// rewind to the beggining of the file
			fluid_player_seek (player, 0);
			// play the midi files, if any
			if (fluid_player_play (player) == FLUID_FAILED) {
				// no file to play; force is_play to FALSE
				is_play = FALSE;
			}
		}
		else
		{
			// stop the midi files, if any
			fluid_player_stop (player);
		}

	}


	// check if "load files" pad has been pressed
	if (same_event(event->buffer,filename[0].ctrl[LOAD])) {
		// set LOAD value to TRUE; it will be set back to false in the main thread, when files are actually loaded 
		is_load=TRUE;
	}


	
	// PROCESS FILE FUNCTIONS : VOL -/+, BPM -/+
	// check if volume down pad has been pressed
	if (same_event(event->buffer,filefunct[0].ctrl[VOLDOWN])) {
		// set VOLUME value to TRUE; it will be set back to false in the main thread, where volume is actually set
		is_volume=TRUE;
		// adjust volume: decrements until is reaches 0
		volume = (volume <= 0) ? 0 : (volume - 1);

		// if volume == 0, then light on volume down pad to indicate we have reached the lower limit
		if (volume == 0) {
				led_filefunct (0, VOLDOWN, ON);
				led_filefunct (0, VOLUP, OFF);
		}
		else {
			// if volume == 2 (default value), then light on both pads, in PENDING mode
			if (volume == 2) {
				led_filefunct (0, VOLDOWN, PENDING);
				led_filefunct (0, VOLUP, PENDING);
			}
			// in other cases, turn light of both pads (voldown and up)
			else {
				led_filefunct (0, VOLDOWN, OFF);
				led_filefunct (0, VOLUP, OFF);
			}
		}
	}

	// check if volume up pad has been pressed
	if (same_event(event->buffer,filefunct[0].ctrl[VOLUP])) {
		// set VOLUME value to TRUE; it will be set back to false in the main thread, where volume is actually set
		is_volume=TRUE;
		// adjust volume: increments until is reaches 1
		volume = (volume >= 10) ? 10 : (volume + 1);

		// if volume == 10, then light on volume up pad to indicate we have reached the higher limit
		if (volume == 10) {
				led_filefunct (0, VOLDOWN, OFF);
				led_filefunct (0, VOLUP, ON);
		}
		else {
			
			// if volume == 2 (default value), then light on both pads, in PENDING mode
			if (volume == 2) {
				led_filefunct (0, VOLDOWN, PENDING);
				led_filefunct (0, VOLUP, PENDING);
			}
			// in other cases, turn light of both pads (voldown and up)
			else {
				led_filefunct (0, VOLDOWN, OFF);
				led_filefunct (0, VOLUP, OFF);
			}
		}
	}

	// check if BPM down pad has been pressed
	if (same_event(event->buffer,filefunct[0].ctrl[BPMDOWN])) {

		// get initial BPM, in case we don't have it yet
		if (initial_bpm == -1) {
			initial_bpm = (fluid_player_get_bpm (player) == FLUID_FAILED) ? 0 : fluid_player_get_bpm (player);
		}

		// get bpm of the file
		bpm = (fluid_player_get_bpm (player) == FLUID_FAILED) ? 0 : fluid_player_get_bpm (player);

		// adjust tempo: decrements until is reaches 0
		bpm = (bpm <= 0) ? 0 : (bpm - 2);
		fluid_player_set_tempo (player, FLUID_PLAYER_TEMPO_EXTERNAL_BPM, bpm);

		// if bpm == 0, then light on bpm down pad to indicate we have reached the lower limit
		if (bpm == 0) {
				led_filefunct (0, BPMDOWN, ON);
				led_filefunct (0, BPMUP, OFF);
		}
		else {
			// if bpm == initial bpm of the file, then light on both pads, in PENDING mode
			if (bpm == initial_bpm) {
				led_filefunct (0, BPMDOWN, PENDING);
				led_filefunct (0, BPMUP, PENDING);
			}
			// in other cases, turn light off on both pads (bpmdown and up)
			else {
				led_filefunct (0, BPMDOWN, OFF);
				led_filefunct (0, BPMUP, OFF);
			}
		}
	}

	// check if BPM up pad has been pressed
	if (same_event(event->buffer,filefunct[0].ctrl[BPMUP])) {

		// get initial BPM, in case we don't have it yet
		if (initial_bpm == -1) {
			initial_bpm = (fluid_player_get_bpm (player) == FLUID_FAILED) ? 0 : fluid_player_get_bpm (player);
		}

		// get bpm of the file
		bpm = (fluid_player_get_bpm (player) == FLUID_FAILED) ? 0 : fluid_player_get_bpm (player);

		// adjust tempo: increments until it reaches 60000000
		bpm = (bpm >= 60000000) ? 60000000 : (bpm + 2);
		fluid_player_set_tempo (player, FLUID_PLAYER_TEMPO_EXTERNAL_BPM, bpm);

		// if bpm == 60000000, then light on bpm up pad to indicate we have reached the higher limit
		if (bpm == 60000000) {
				led_filefunct (0, BPMDOWN, OFF);
				led_filefunct (0, BPMUP, ON);
		}
		else {
			// if bpm == initial bpm of the file, then light on both pads, in PENDING mode
			if (bpm == initial_bpm) {
				led_filefunct (0, BPMDOWN, PENDING);
				led_filefunct (0, BPMUP, PENDING);
			}
			// in other cases, turn light off on both pads (bpmdown and up)
			else {
				led_filefunct (0, BPMDOWN, OFF);
				led_filefunct (0, BPMUP, OFF);
			}
		}
	}

	// check if BEAT pad has been pressed
	if (same_event(event->buffer,filefunct[0].ctrl[BEAT])) {
		beat_process ();
	}
}


// process managing external switch and LED
int gpio_process () {

	// get current time
	now = micros ();

	// test if GPIO is enabled
	if (gpio_state == ON) {

		// test switch value; if switch is pressed then value is LOW
		if (gpioRead (SWITCH_GPIO) == OFF) {
			// anti_bounce mechanism: make sure the switch is not "bouncing", causing repeated ON-OFF in a short period
			// no bounce if previous is 0
			if ((previous == 0) || ((now-previous) >= ANTIBOUNCE_US))
			{
				previous_led = now;			// set time when led has been put on
				gpioWrite (LED_GPIO, ON);	// turn LED ON
				return TRUE;				// switch pressed, no bounce : exit function with press	OK
			}
		}

		// check when to turn LED off : it is turned off when led is on for more than TIMEON_US
	    if ((now - previous_led) > TIMEON_US) {
			gpioWrite (LED_GPIO, OFF);	// turn LED OFF
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


// process callback called to process midi player ticks in realtime
// we use this function to convert midi file ticks (PPQ ticks per quarrter note)
// into MIDI_CLOCK ticks (24 ticks per quarter note) to drive external midi systems
int handle_tick(void *data, int tick) {

	fluid_player_t* player;
	int index_pulse;
	int i;
	float ppq_per_midi_clock;
	int end_tick;


	// define data as being a pointer to player
	player = (fluid_player_t*) data;
		

	return FLUID_OK;
}
