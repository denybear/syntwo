/** @file globals.h
 *
 * @brief This file the global variables for the program
 *
 */

// global variables

// Fluidsynth variables
extern fluid_settings_t* settings;
extern fluid_synth_t* synth;
extern fluid_midi_driver_t* mdriver;
extern fluid_player_t* player;
extern fluid_audio_driver_t* adriver;


// define if gpio is running for external beat switch and LED
extern int gpio_state;      // OFF = gpio OFF: ON = GPIO ON 
extern uint64_t previous_led;  // time when switch was set as on

/* load (midi and SF2 files) & play (midi file) globals */
extern int midi_load, sf2_load;
// file number for midi and sf2 files
extern uint8_t new_midi_num, new_sf2_num;				// new file number to be loaded
extern uint8_t current_midi_num, current_sf2_num;		// current file number that is currently loaded

extern int sf2_id;		// id of sf2 file currently loaded

/* volume and BPM */
extern int bpm;
extern int initial_bpm;
extern int volume;

/* beat */
extern uint64_t now;       // time now
extern uint64_t previous;  // time when "beat" key was last pressed

/* definition of the MIDI controler controls */
extern channel_t channel [NB_CHANNEL] [NB_RECSHIFT];		// 8 channel control * 2 (without shift and with shift; REC key)
extern button_t cycle;							// cycle button used as shift key
extern button_t track_l [NB_CYCSHIFT];			// track left button; could be used with shift (cycle) key for SF2 left
extern button_t track_r [NB_CYCSHIFT];			// track right button; could be used with shift (cycle) key for SF2 right
extern button_t rwd [NB_CYCSHIFT];			    // rewind: could be used with shift
extern button_t fwd [NB_CYCSHIFT];			    // forward: could be used with shift
extern button_t play, stop, record;
extern button_t set, marker_l, marker_r;
