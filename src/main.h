/** @file main.h
 *
 * @brief This file contains global variables definition
 *
 */


/********************/
/* global variables */
/********************/

// Fluidsynth variables
fluid_settings_t* settings;
fluid_synth_t* synth;
fluid_midi_driver_t* mdriver;
fluid_player_t* player;
fluid_audio_driver_t* adriver;

// define if gpio is running for external beat switch and LED
int gpio_state;     // OFF = gpio OFF: ON = GPIO ON
uint64_t previous_led;  // time when switch was set as on

/* load (midi and SF2 files) & play (midi file) globals */
int midi_load, sf2_load;
// file number for midi and sf2 files
uint8_t midi_num, sf2_num;

int is_play;
int sf2_id;		// id of sf2 file currently loaded

/* volume and BPM */
int bpm;
int initial_bpm;
int volume;
int is_volume;

/* PPQ */
int ppq;

/* beat */
uint64_t now;       // time now
uint64_t previous;  // time when "beat" key was last pressed

/* definition of the MIDI controler controls */
channel_t channel [NB_CHANNEL] [NB_RECSHIFT];		// 8 channel control * 2 (without shift and with shift; REC key)
button_t cycle;							// cycle button used as shift key
button_t track_l [NB_CYCSHIFT];			// track left button; could be used with shift (cycle) key for SF2 left
button_t track_r [NB_CYCSHIFT];			// track right button; could be used with shift (cycle) key for SF2 right
button_t rwd [NB_CYCSHIFT];			    // rewind: could be used with shift
button_t fwd [NB_CYCSHIFT];			    // forward : could be used with shift
button_t play, stop, record;
button_t set, marker_l, marker_r;
