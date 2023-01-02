/** @file main.c
 *
 * @brief This is the main file for the syntwo program. It uses fluidsynth to play midi files.
 *
 */

#include "types.h"
#include "main.h"
#include "config.h"
#include "process.h"


/*************/
/* functions */
/*************/
// you need to have root priviledges for it to work
static int init_gpio ()
{
	if (gpioInitialise() == PI_INIT_FAILED) {
    	fprintf(stderr, "pigpio initialisation failed\n");
    	return OFF;
	}

	/* Set GPIO modes */
	gpioSetMode(LED_GPIO, PI_OUTPUT);
	gpioWrite (LED_GPIO, OFF);					// at start, LED is off

	gpioSetMode(SWITCH_GPIO, PI_INPUT);
	gpioSetPullUpDown(SWITCH_GPIO, PI_PUD_UP);	// Sets a pull-up
	// benefits of pull-up is that way, no voltage are input in the pins; pins are only put to GND
	return ON;
}

// you need to have root priviledges for it to work
static int kill_gpio ()
{
	gpioTerminate ();
}


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
	memset (rewind, 0, NB_CYCSHIFT * sizeof (button_t));
	memset (forward, 0, NB_CYCSHIFT * sizeof (button_t));
	memset (&cycle, 0, sizeof (button_t));
	memset (&play, 0, sizeof (button_t));
	memset (&stop, 0, sizeof (button_t));
	memset (&record, 0, sizeof (button_t));
	memset (&set, 0, sizeof (button_t));
	memset (&marker_l, 0, sizeof (button_t));
	memset (&marker_r, 0, sizeof (button_t));

	// init clock sending
	send_clock = NO_CLOCK;

	// clear load/play flags
	// is_load = TRUE allows to load default files (00_*) at startup
	is_load = TRUE;
	is_play = FALSE;
	sf2_id = 0;			// set arbitrary value for sf2_id (current loaded soundfile id)
	
	// function flags
	volume = 2;
	is_volume = TRUE;	// force setting the volume at startup
	bpm = 0	;			// bpm is only set when file is playing
	initial_bpm = -1;
	now = 0;			// used for automated tempo adjustment (at press of switch)
	previous = 0;
	previous_led = 0;	// time when LED was turned ON
}


static void signal_handler ( int sig )
{
	kill_gpio ();

	delete_fluid_audio_driver(adriver);
	delete_fluid_player(player);
	delete_fluid_synth(synth);
	delete_fluid_settings(settings);
    delete_fluid_midi_driver(mdriver);

	fprintf ( stderr, "signal received, exiting ...\n" );
	exit ( 0 );
}


/* usage: syntwo*/

int main ( int argc, char *argv[] )
{
	int i,j;

	// Fluidsynth variables
	fluid_settings_t* settings;
	fluid_synth_t* synth;
    fluid_midi_driver_t* mdriver;
	// note that fluid_player_t* player; is declared as global variable as we need it in jack's process () thread
	fluid_audio_driver_t* adriver;



	
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

	// init fluidsynth
	settings = new_fluid_settings();
	synth = new_fluid_synth(settings);
    mdriver = new_fluid_midi_driver(settings, handle_midi_event, NULL);		// callback called every time a midi event is received by Fluidsynth
	// alsa_raw as audio driver
	// sample rate as the one defined in jack
	fluid_settings_setstr(settings, "audio.driver", "alsa_raw");
	fluid_settings_setstr(settings, "synth.sample-rate", 48000);

	// start the synthesizer thread
	adriver = new_fluid_audio_driver(settings, synth);

	// load default soundfont
	if (fluid_is_soundfont(DEFAULT_SF2)) {
		fluid_synth_sfload(synth, DEFAULT_SF2, 1);
	}

	// create new player, but don't load anything for now
	player = new_fluid_player(synth);



	/*************/
	/* MAIN LOOP */
	/*************/

	/* switch all leds off for all functions */

	// at start, we use default volume (2); light on the volume pads to indicate this to the user

	/* keep running until the transport stops */
	while (1)
	{
		// check if user has loaded the LOAD button to load midi and SF2 file
		if (is_load) {

			// make sure no file is playing to allow load of new file !
			if ((fluid_player_get_status (player)== FLUID_PLAYER_DONE) || (fluid_player_get_status (player)== FLUID_PLAYER_READY)) {
			
				// get name of requested midi file from directory
				if (get_full_filename (name, name_to_byte (&filename [0]), "./songs/") == TRUE) {
					// if a file exists
					if (fluid_is_midifile(name)) {
						
						// delete current fluid player
						delete_fluid_player (player);
						// create new player
						player = new_fluid_player(synth);
						// set player callback at tick
						fluid_player_set_tick_callback (player, handle_tick, (void *) player);

						// get new ppq value
						ppq = get_division (name); 
						// load midi file
						fluid_player_add(player, name);
						// set endless looping of current file
						fluid_player_set_loop (player, -1);

						// initial bpm of the file is set to -1 to force reading of initial bpm if bpm pads are pressed
						initial_bpm = -1;
						now = 0;			// used for automated tempo adjustment (at press of switch)
						previous = 0;

						// we are at initial BPM; set the 2 BPM pads accordingly
						led_filefunct (0, BPMDOWN, PENDING);
						led_filefunct (0, BPMUP, PENDING);

					}
				}

				// get name of requested SF2 file from directory
				if (get_full_filename (name, name_to_byte (&filename [1]), "./soundfonts/") == TRUE) {
					// if a file exists
					if (fluid_is_soundfont(name)) {
						// unload previously loaded soundfont
						// this is to prevent memory issues (lack of memory)
						fluid_synth_sfunload (synth, sf2_id, TRUE);
						// load new sf2 file
						sf2_id = fluid_synth_sfload(synth, name, TRUE);
					}
				}
			}
			
			// load is done; set to FALSE
			is_load = FALSE;
			// load led OFF
			led_filename (0, LOAD, is_load);
		}

		// check if user has pressed volume pads; this part is also executed at startup
		if (is_volume) {
			// set gain: 0 < gain < 1.0 (default = 0.2)
			fluid_settings_setnum (settings, "synth.gain", (float) volume/10.0f);
			// volume setting is done; set to FALSE
			is_volume = FALSE;
			// no need to light on/off the pads, this is done in the process thread
		}


		/*****************************************************************************/
		/* At very first, process external beat switch by checking if pressed or not */
		/*****************************************************************************/
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
