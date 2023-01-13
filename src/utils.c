/** @file process.c
 *
 * @brief The process callback for this JACK application is called in a
 * special realtime thread once for each audio cycle.
 *
 */

#include "types.h"
#include "globals.h"
#include "config.h"
#include "process.h"
#include "utils.h"
#include "gpio.h"

// in the given directory, look for filename starting with number, and return corresponding full name
// returns FALSE if no file found, TRUE if file is found 
int get_full_filename (char * name, unsigned char number, char * directory) {

	DIR *dir;
	struct dirent *ent;
	char st1[3], st2[3];

	// convert number into string (in lowercases and uppercases)
	sprintf (st1, "%02x", number);
	sprintf (st2, "%02X", number);
	if ((dir = opendir (directory)) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) {
			// check if file starts with number; allow mixing lower and upper cases
			if (((st1[0] == ent->d_name [0]) || (st2[0] == ent->d_name [0])) && ((st1[1] == ent->d_name [1]) || (st2[1] == ent->d_name [1]))) {
				// copy full file name (including directory) into result variable
				strcpy (name, directory);
				strcat (name, ent->d_name);
				closedir (dir);
				return TRUE;
			}
		}
	// could not find a file starting with number
	closedir (dir);
	return FALSE;
	}

	else {
		// could not open directory
		fprintf ( stderr, "Directory not found.\n" );
		return FALSE;
	}
}


// Check if file number (ie. filename) of midi & sf2 files has changed compared to what is currently used/played. So yes, then load new files (either midi, either SF2, either both)
int load_midi_sf2 () {

	// string containing : directory + filename
	char name [300];

	// make sure no file is playing to allow load of new files !
	if ((fluid_player_get_status (player)== FLUID_PLAYER_DONE) || (fluid_player_get_status (player)== FLUID_PLAYER_READY)) {

		// check if user has requested load midi of midi file and that new file to download is not the same as current
		if (new_midi_num != current_midi_num) {
		
			// get name of requested midi file from directory
			if (get_full_filename (name, new_midi_num, "./songs/") == TRUE) {
				// if a file exists
				if (fluid_is_midifile(name)) {
printf ("midi:%s\n", name);
					// delete current fluid player
					delete_fluid_player (player);
					// create new player
					player = new_fluid_player(synth);

					// load midi file
					fluid_player_add(player, name);

					// set endless looping of current file
					//fluid_player_set_loop (player, -1);

					// loading has been done
					current_midi_num = new_midi_num;

					// initial bpm of the file is set to -1 to force reading of initial bpm if bpm pads are pressed
					initial_bpm = -1;
					now = 0;			// used for automated tempo adjustment (at press of switch)
					previous = 0;

					// clear table of time markers
					memset (&marker [0], 0, sizeof (int) * NB_MARKER);
					marker_pos = 0;

					// we are at initial BPM, set leds accordingly
					// this is useless as we cannot control the leds for now
					led (&rwd[0], ON);
					led (&fwd[0], ON);
				}
			}
		}

		if (new_sf2_num != current_sf2_num) {
			// get name of requested SF2 file from directory
			if (get_full_filename (name, new_sf2_num, "./soundfonts/") == TRUE) {
				// if a file exists
				if (fluid_is_soundfont(name)) {
printf ("sf2:%s\n", name);
					// unload previously loaded soundfont
					// this is to prevent memory issues (lack of memory)
					// however make sure we don't unload the only soundfont in memory
					// in theory we should have at least 1 soundfont all the time in memory (default SF2)
					if (fluid_synth_sfcount (synth) > 1) fluid_synth_sfunload (synth, sf2_id, TRUE);
					// load new sf2 file
					sf2_id = fluid_synth_sfload(synth, name, TRUE);

					// loading has been done
					current_sf2_num = new_sf2_num;
				}
			}
		}
	}
}


// Convert seconds to microseconds
#define SEC_TO_US(sec) ((sec)*1000000)
// Convert nanoseconds to microseconds
#define NS_TO_US(ns)    ((ns)/1000)

// Get a time stamp in microseconds.
uint64_t micros () {
	
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    uint64_t us = SEC_TO_US((uint64_t)ts.tv_sec) + NS_TO_US((uint64_t)ts.tv_nsec);
    return us;
}


// Switch LEDs of midi device buttons on/off
// this function is useless as we cannot control the leds with fluidsynth for now (no way to send outgoing midi messages to hardware)
void led (button_t* button, int on_off) {

	if (on_off) {
		// LED ON
		// send_midi (button->led_on);
	}
	else {
		// LED OFF
		// send_midi (button->led_off);
	}
}
