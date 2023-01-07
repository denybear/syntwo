/** @file main.c
 *
 * @brief This is the main file for the syntwo program. It uses fluidsynth to play midi files.
 *
 */

#include "types.h"
#include "main.h"
#include "config.h"
#include "process.h"
#include "utils.h"
#include "gpio.h"


/*************/
/* functions */
/*************/

static void init_globals ( )
{
	int i,j;

	/******************************/
	/* INIT SOME GLOBAL VARIABLES */
	/******************************/

	/* clear structure that will get control data */
/*	for (i = 0; i<NB_CHANNEL; i++) {
		for (j = 0; j<NB_RECSHIFT; j++) {
			memset (&channel[i][j], 0, sizeof (channel_t));
		}
	}*/
	memset (channel, 0, NB_CHANNEL * NB_RECSHIFT * sizeof (channel_t));
	memset (track_l, 0, NB_CYCSHIFT * sizeof (button_t));
	memset (track_r, 0, NB_CYCSHIFT * sizeof (button_t));
	memset (rwd, 0, NB_CYCSHIFT * sizeof (button_t));
	memset (fwd, 0, NB_CYCSHIFT * sizeof (button_t));
	memset (&cycle, 0, sizeof (button_t));
	memset (&play, 0, sizeof (button_t));
	memset (&stop, 0, sizeof (button_t));
	memset (&record, 0, sizeof (button_t));
	memset (&set, 0, sizeof (button_t));
	memset (&marker_l, 0, sizeof (button_t));
	memset (&marker_r, 0, sizeof (button_t));

	// clear load/play flags
	// is_load = TRUE allows to load default files (00_*) at startup
	midi_load = TRUE;
	sf2_load = TRUE;
	new_midi_num = 0;		// file 00 as default for both midi and sf2
	new_sf2_num = 0;
	current_midi_num = 1;		// set current file at 01 to force loading of files number 0 at startup
	current_sf2_num = 1;
	sf2_id = 0;			// set arbitrary value for sf2_id (current loaded soundfile id)

	// function flags
	volume = 2;
	is_volume = TRUE;	// force setting the volume at startup
	// we are at initial volume, set leds accordingly
	// this is useless as we cannot control the leds for now
	led (&rwd[1], ON);
	led (&fwd[1], ON);

	// set bpm variables for bpm keys & tap-tempo / beat switch
	bpm = 0	;			// bpm is only set when file is playing
	initial_bpm = -1;
	now = 0;			// used for automated tempo adjustment (at press of switch)
	previous = 0;
	previous_led = 0;	// time when LED was turned ON
}


static void signal_handler ( int sig )
{
	kill_gpio ();

    // wait for playback termination
    fluid_player_join(player);


	// for fluidsynth to stop properly, one must do these steps backwards
	// 1. create settings
 	// 2. set midi driver settings
 	// 3. set audio driver settings
 	// 4. create synth
 	// 5. load soundfont
 	// 6. create audio driver
 	// 7. create midi driver

	delete_fluid_player(player);
	delete_fluid_midi_driver(mdriver);
	delete_fluid_audio_driver(adriver);
	delete_fluid_synth(synth);
	delete_fluid_settings(settings);

	fprintf ( stderr, "signal received, exiting ...\n" );
	exit ( 0 );
}


/* usage: syntwo*/

int main ( int argc, char *argv[] )
{
	int i,j;
	// string containing : directory + filename
	char name [1000];

	/* install a signal handler to properly quit */
#ifdef WIN32
	signal ( SIGINT, signal_handler );
	signal ( SIGABRT, signal_handler );
	signal ( SIGTERM, signal_handler );
#else
	signal ( SIGQUIT, signal_handler );
	signal ( SIGTERM, signal_handler );
	signal ( SIGHUP, signal_handler );
	signal ( SIGINT, signal_handler );
#endif


	// init GPIO to enable external "beat" switch (tap tempo)
	gpio_state = init_gpio ();

	// init global variables and set midi message values
	init_globals ();
	read_config ();

	// For fluidsynth, things mut be done in the following order
	// 1. create settings
 	// 2. set midi driver settings
 	// 3. set audio driver settings
 	// 4. create synth
 	// 5. load soundfont
 	// 6. create audio driver
 	// 7. create midi driver

	// init fluidsynth
	// create settings
	settings = new_fluid_settings();

	// settings for fluidsynth midi and audio

	fluid_settings_setstr(settings, "midi.driver", "alsa_raw");
// METTRE LE NUMERO DE DEVICE EN ARGV???
	fluid_settings_setstr(settings, "midi.alsa.device", "hw:2,0,0");

	fluid_settings_setstr(settings, "audio.driver", "alsa");
	// fluid_settings_setstr(settings, "synth.sample-rate", "48000.0");

	// create synth
	synth = new_fluid_synth(settings);

	// load default soundfont
	if (fluid_is_soundfont(DEFAULT_SF2)) {
		


/********************/
/* BUG DS SYNTI???  */
/********************/	
// correction ci dessous: set sf2_id

		
		
		sf2_id = fluid_synth_sfload(synth, DEFAULT_SF2, 1);
	}

	// start audio driver
	adriver = new_fluid_audio_driver(settings, synth);

	// start midi driver
    mdriver = new_fluid_midi_driver(settings, handle_midi_event, (void *) synth);		// callback called every time a midi event is received from HW device

	// create new player, but don't load anything for now
	player = new_fluid_player(synth);
// callback for handling midi events from midi file
//fluid_player_set_playback_callback	(player, handle_midi_event, (void *) synth);

	// load default midi and sf2 files before main loop
	load_midi_sf2 ();

	/*************/
	/* MAIN LOOP */
	/*************/

	// at start, we use default volume (2); light on the volume pads to indicate this to the user
	// also light to indicate we are at standard bpm

	while (1)
	{
		// process external beat switch by checking if pressed or not
		if (gpio_process () == TRUE) beat_process ();


#ifdef WIN32
		Sleep ( 1000 );
#else
		sleep ( 1 );
#endif
	}

	// terminate
	signal_handler (0);
}
