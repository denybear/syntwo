/** @file process.c
 *
 * @brief The process callback for this application is called when midi messages are received
 *
 */

#include "types.h"
#include "globals.h"
#include "config.h"
#include "process.h"
#include "utils.h"
#include "gpio.h"

// generic process function called everytime a known midi command is received
int process (void *control, uint8_t *data)
{
//	printf ("in PROCESS function: %02X %02X %02X\n", data[0], data [1], data [2]);

	return FLUID_OK;
}

// process function called everytime slide is actionned
int process_slider (void *control, uint8_t *data)
{
	slider_t *ctrl;
	ctrl = control;

//	printf ("SLIDER: %02X %02X %02X\n", data[0], data [1], data [2]);

	// get value from the midi control
	ctrl->value = data[2];
	// send CC7 (sound control) to synthetizer
	// data[1] is the channel number
	fluid_synth_cc (synth, data[1], 7, ctrl->value);

	return FLUID_OK;
}

// process function called everytime slide is actionned for channels > 8
int process_slider_shift (void *control, uint8_t *data)
{
	slider_t *ctrl;
	ctrl = control;

//	printf ("SLIDER_SHIFT: %02X %02X %02X\n", data[0], data [1], data [2]);

	// get value from the midi control
	ctrl->value = data[2];
	// send CC7 (sound control) to synthetizer
	// data[1] is the channel number
	fluid_synth_cc (synth, data[1]+0x08, 7, ctrl->value);

	return FLUID_OK;
}

// process function called everytime knob is actionned
int process_knob (void *control, uint8_t *data)
{
	knob_t *ctrl;
	ctrl = control;

//	printf ("KNOB: %02X %02X %02X\n", data[0], data [1], data [2]);

	// get value from the midi control
	ctrl->value = data[2];
	// send CC8 (balance) to synthetizer
	// data[1] is the channel number
	fluid_synth_cc (synth, data[1]-0x10, 8, ctrl->value);

	return FLUID_OK;
}

// process function called everytime knob is actionned for channels > 8
int process_knob_shift (void *control, uint8_t *data)
{
	knob_t *ctrl;
	ctrl = control;

//	printf ("KNOB_SHIFT: %02X %02X %02X\n", data[0], data [1], data [2]);

	// get value from the midi control
	ctrl->value = data[2];
	// send CC8 (balance) to synthetizer
	// data[1] is the channel number
	fluid_synth_cc (synth, data[1]-0x10+0x08, 8, ctrl->value);

	return FLUID_OK;
}

// process function called everytime solo button is pressed
// TOGGLE MODE ON
int process_solo (void *control, uint8_t *data)
{
	int i, j, k, cc, ch;
	button_t *ctrl;
	ctrl = control;

//	printf ("SOLO: %02X %02X %02X\n", data[0], data [1], data [2]);

	// get channel number
	ch = data [1] - 0x20;
	
	// get value from the midi control: 0 or 1 (OFF or ON)
	ctrl->value = data[2] & 0x01;		// &0x01 as value in midi message is 7F in case of ON
	// useless as we cannot control leds
	// switch led on/off accordingly
	led (ctrl, ctrl->value);

	if (ctrl->value) {
		// solo ON
		// get current value of all CC7 channel; if no value set, then fix arbitrary value to 64
		for (i = 0; i<NB_CHANNEL; i++) {
			for (j=0; j<NB_RECSHIFT; j++) {
				k = i + (j * 8);

				if (fluid_synth_get_cc (synth, k, 7, &cc) != FLUID_OK) cc = 0x40;
				// save current CC value to the corresponding slider
				channel[i][j].slider.value_s = cc;
				// send CC7 (sound control) to synthetizer as 0 to mute the channel
				// for all channels but ch (current channel number)
				if (ch != k) {
					fluid_synth_cc (synth, k, 7, 0);
					channel[i][j].slider.value = 0;		// set actual value for this slider
				}
			}
		}
	}
	else {
		// solo OFF
		// get current CC value from all sliders
		for (i = 0; i<NB_CHANNEL; i++) {
			for (j=0; j<NB_RECSHIFT; j++) {
				k = i + (j * 8);

				// get current CC value from corresponding slider
				cc = channel[i][j].slider.value_s;
				// send CC7 (sound control) to synthetizer as cc value to unmute the channel
				// for all channels but ch (current channel number)
				if (ch != k) {
					fluid_synth_cc (synth, k, 7, cc);
					channel[i][j].slider.value = cc;		// set actual value for this slider
				}
			}
		}
	}

	return FLUID_OK;
}

// process function called everytime solo button is pressed for channels > 8
// TOGGLE MODE ON
int process_solo_shift (void *control, uint8_t *data)
{
	int i, j, k, cc, ch;
	button_t *ctrl;
	ctrl = control;

//	printf ("SOLO_SHIFT: %02X %02X %02X\n", data[0], data [1], data [2]);

	// get channel number
	ch = data [1] - 0x20 + 0x08;
	
	// get value from the midi control: 0 or 1 (OFF or ON)
	ctrl->value = data[2] & 0x01;		// &0x01 as value in midi message is 7F in case of ON
	// useless as we cannot control leds
	// switch led on/off accordingly
	led (ctrl, ctrl->value);

	if (ctrl->value) {
		// solo ON
		// get current value of all CC7 channel; if no value set, then fix arbitrary value to 64
		for (i = 0; i<NB_CHANNEL; i++) {
			for (j=0; j<NB_RECSHIFT; j++) {
				k = i + (j * 8);

				if (fluid_synth_get_cc (synth, k, 7, &cc) != FLUID_OK) cc = 0x40;
				// save current CC value to the corresponding slider
				channel[i][j].slider.value_s = cc;
				// send CC7 (sound control) to synthetizer as 0 to mute the channel
				// for all channels but ch (current channel number)
				if (ch != k) {
					fluid_synth_cc (synth, k, 7, 0);
					channel[i][j].slider.value = 0;		// set actual value for this slider
				}
			}
		}
	}
	else {
		// solo OFF
		// get current CC value from all sliders
		for (i = 0; i<NB_CHANNEL; i++) {
			for (j=0; j<NB_RECSHIFT; j++) {
				k = i + (j * 8);

				// get current CC value from corresponding slider
				cc = channel[i][j].slider.value_s;
				// send CC7 (sound control) to synthetizer as cc value to unmute the channel
				// for all channels but ch (current channel number)
				if (ch != k) {
					fluid_synth_cc (synth, k, 7, cc);
					channel[i][j].slider.value = cc;		// set actual value for this slider
				}
			}
		}
	}

	return FLUID_OK;
}

// process function called everytime mute button is pressed
// TOGGLE MODE ON
int process_mute (void *control, uint8_t *data)
{
	int i, cc;
	button_t *ctrl;
	ctrl = control;
	
//	printf ("MUTE: %02X %02X %02X\n", data[0], data [1], data [2]);
	// get channel number
	i = data [1] - 0x30;
	
	// get value from the midi control: 0 or 1 (OFF or ON)
	ctrl->value = data[2] & 0x01;		// &0x01 as value in midi message is 7F in case of ON
	// useless as we cannot control leds
	// switch led on/off accordingly
	led (ctrl, ctrl->value);

	if (ctrl->value) {
		// mute ON
		// get current value of CC7 of channel; if no value set, then fix arbitrary value to 64
		if (fluid_synth_get_cc (synth, i, 7, &cc) != FLUID_OK) cc = 0x40;
		// save current CC value to the corresponding slider
		channel[i][0].slider.value_m = cc;
		// send CC7 (sound control) to synthetizer as 0 to mute the channel
		// i is the channel number
		fluid_synth_cc (synth, i, 7, 0);
		channel[i][0].slider.value = 0;		// set actual value for this slider
	}
	else {
		// mute OFF
		// get current CC value from the corresponding slider
		cc = channel[i][0].slider.value_m;
		// send CC7 (sound control) to synthetizer as cc value to unmute the channel
		// i is the channel number
		fluid_synth_cc (synth, i, 7, cc);
		channel[i][0].slider.value = cc;		// set actual value for this slider
	}

	return FLUID_OK;
}

// process function called everytime mute button is pressed for channels > 8
// TOGGLE MODE ON
int process_mute_shift (void *control, uint8_t *data)
{
	int i, cc;
	button_t *ctrl;
	ctrl = control;
	
//	printf ("MUTE_SHIFT: %02X %02X %02X\n", data[0], data [1], data [2]);
	// get channel number
	i = data [1] - 0x30 + 0x08;
	
	// get value from the midi control: 0 or 1 (OFF or ON)
	ctrl->value = data[2] & 0x01;		// &0x01 as value in midi message is 7F in case of ON
	// useless as we cannot control leds
	// switch led on/off accordingly
	led (ctrl, ctrl->value);

	if (ctrl->value) {
		// mute ON
		// get current value of CC7 of channel; if no value set, then fix arbitrary value to 64
		if (fluid_synth_get_cc (synth, i, 7, &cc) != FLUID_OK) cc = 0x40;
		// save current CC value to the corresponding slider
		channel[i-0x08][1].slider.value_m = cc;
		// send CC7 (sound control) to synthetizer as 0 to mute the channel
		// i is the channel number
		fluid_synth_cc (synth, i, 7, 0);
		channel[i-0x08][1].slider.value = 0;		// set actual value for this slider

	}
	else {
		// mute OFF
		// get current CC value from the corresponding slider
		cc = channel[i-0x08][1].slider.value_m;
		// send CC7 (sound control) to synthetizer as cc value to unmute the channel
		// i is the channel number
		fluid_synth_cc (synth, i, 7, cc);
		channel[i-0x08][1].slider.value_m = cc;		// set actual value for this slider
	}

	return FLUID_OK;
}

// process function called everytime rec button is pressed
// TOGGLE MODE ON
int process_rec (void *control, uint8_t *data)
{
	button_t *ctrl;
	ctrl = control;

//	printf ("REC: %02X %02X %02X\n", data[0], data [1], data [2]);

	// get value from the midi control: 0 or 1 (OFF or ON)
	ctrl->value = data[2] & 0x01;		// &0x01 as value in midi message is 7F in case of ON
	// switch led on/off accordingly
	led (ctrl, ctrl->value);

	return FLUID_OK;
}

// process function called everytime cycle button is pressed
// TOGGLE MODE ON
int process_cycle (void *control, uint8_t *data)
{
	button_t *ctrl;
	ctrl = control;

//	printf ("CYCLE: %02X %02X %02X\n", data[0], data [1], data [2]);

	// get value from the midi control: 0 or 1 (OFF or ON)
	ctrl->value = data[2] & 0x01;		// &0x01 as value in midi message is 7F in case of ON
	// switch led on/off accordingly
	led (ctrl, ctrl->value);

	return FLUID_OK;
}

// process function called everytime track_l button is pressed
// MOMENTARY MODE ON
int process_track_l (void *control, uint8_t *data)
{
	button_t *ctrl;
	ctrl = control;

//	printf ("TRACK_L: %02X %02X %02X\n", data[0], data [1], data [2]);

	// do something only if button is pressed (but don't do anything if released)
	if (data [2] != 0) {
		// decrease midi file number until it reaches 0
		if (new_midi_num > 0) new_midi_num--; 
	}

	// no need to update value of ctrl (it is not used)
	// no need to set any led (there are no led for this key)

	return FLUID_OK;
}

// process function called everytime track_r button is pressed
// MOMENTARY MODE ON
int process_track_r (void *control, uint8_t *data)
{
	button_t *ctrl;
	ctrl = control;

//	printf ("TRACK_R: %02X %02X %02X\n", data[0], data [1], data [2]);

	// do something only if button is pressed (but don't do anything if released)
	if (data [2] != 0) {
		// increase midi file number until it reaches FF
		if (new_midi_num < 0xFF) new_midi_num++; 
	}

	// no need to update value of ctrl (it is not used)
	// no need to set any led (there are no led for this key)

	return FLUID_OK;
}

// process function called everytime track_l button is pressed and shift is ON
// MOMENTARY MODE ON
int process_track_l_shift (void *control, uint8_t *data)
{
	button_t *ctrl;
	ctrl = control;

//	printf ("TRACK_L_SHIFT: %02X %02X %02X\n", data[0], data [1], data [2]);

	// do something only if button is pressed (but don't do anything if released)
	if (data [2] != 0) {
		// decrease sf2 file number until it reaches 0
		if (new_sf2_num > 0) new_sf2_num--; 
	}

	// no need to update value of ctrl (it is not used)
	// no need to set any led (there are no led for this key)

	return FLUID_OK;
}

// process function called everytime track_r button is pressed and shift is ON
// MOMENTARY MODE ON
int process_track_r_shift (void *control, uint8_t *data)
{
	button_t *ctrl;
	ctrl = control;

//	printf ("TRACK_R_SHIFT: %02X %02X %02X\n", data[0], data [1], data [2]);

	// do something only if button is pressed (but don't do anything if released)
	if (data [2] != 0) {
		// increase soundfont file number until it reaches FF
		if (new_sf2_num < 0xFF) new_sf2_num++; 
	}

	// no need to update value of ctrl (it is not used)
	// no need to set any led (there are no led for this key)

	return FLUID_OK;
}

//*******************
// CHECK IF IT WORKS WHEN THE FILE IS NOT PLAYING
//*******************
// process function called everytime rwd button is pressed
// MOMENTARY MODE ON
int process_rwd (void *control, uint8_t *data)
{
	button_t *ctrl;
	ctrl = control;

//	printf ("RWD: %02X %02X %02X\n", data[0], data [1], data [2]);

	// do something only if button is pressed (but don't do anything if released)
	if (data [2] != 0) {
		// get initial BPM, in case we don't have it yet
		if (initial_bpm == -1) {
			initial_bpm = (fluid_player_get_bpm (player) == FLUID_FAILED) ? 0 : fluid_player_get_bpm (player);
		}

		// get bpm of the file
		bpm = (fluid_player_get_bpm (player) == FLUID_FAILED) ? 0 : fluid_player_get_bpm (player);

		// adjust tempo: decrements until is reaches 0
		bpm = (bpm <= 0) ? 0 : (bpm - 2);
		fluid_player_set_tempo (player, FLUID_PLAYER_TEMPO_EXTERNAL_BPM, bpm);

		// this part is useless as we cannot control the leds for now
		// if bpm == 0, then light on bpm down pad to indicate we have reached the lower limit
		if (bpm == 0) {
				led (ctrl, ON);
				led (&fwd[0], OFF);		// this is a bit ugly
		}
		else {
			// if bpm == initial bpm of the file, then light on both pads, in PENDING mode
			if (bpm == initial_bpm) {
				led (ctrl, ON);
				led (&fwd[0], ON);		// this is a bit ugly
			}
			// in other cases, turn light off on both pads (bpmdown and up)
			else {
				led (ctrl, OFF);
				led (&fwd[0], OFF);		// this is a bit ugly
			}
		}
	}

	// no need to update value of ctrl (it is not used)

	return FLUID_OK;
}

//*******************
// CHECK IF IT WORKS WHEN THE FILE IS NOT PLAYING
//*******************
// process function called everytime fwd button is pressed
// MOMENTARY MODE ON
int process_fwd (void *control, uint8_t *data)
{
	button_t *ctrl;
	ctrl = control;

//	printf ("FWD: %02X %02X %02X\n", data[0], data [1], data [2]);

	// do something only if button is pressed (but don't do anything if released)
	if (data [2] != 0) {
		// get initial BPM, in case we don't have it yet
		if (initial_bpm == -1) {
			initial_bpm = (fluid_player_get_bpm (player) == FLUID_FAILED) ? 0 : fluid_player_get_bpm (player);
		}

		// get bpm of the file
		bpm = (fluid_player_get_bpm (player) == FLUID_FAILED) ? 0 : fluid_player_get_bpm (player);

		// adjust tempo: increments until it reaches 60000000
		bpm = (bpm >= 60000000) ? 60000000 : (bpm + 2);
		fluid_player_set_tempo (player, FLUID_PLAYER_TEMPO_EXTERNAL_BPM, bpm);

		// this part is useless as we cannot control the leds for now
		// if bpm == 60000000, then light on bpm up pad to indicate we have reached the higher limit
		if (bpm == 60000000) {
				led (&rwd[0], OFF);		// this is a bit ugly
				led (ctrl, ON);
		}
		else {
			// if bpm == initial bpm of the file, then light on both pads, in PENDING mode
			if (bpm == initial_bpm) {
				led (&rwd[0], ON);		// this is a bit ugly
				led (ctrl, ON);
			}
			// in other cases, turn light off on both pads (bpmdown and up)
			else {
				led (&rwd[0], OFF);		// this is a bit ugly
				led (ctrl, OFF);
			}
		}
	}

	// no need to update value of ctrl (it is not used)

	return FLUID_OK;
}

// process function called everytime rwd button is pressed and shift is ON
// MOMENTARY MODE ON
int process_rwd_shift (void *control, uint8_t *data)
{
	button_t *ctrl;
	ctrl = control;

//	printf ("RWD_SHIFT: %02X %02X %02X\n", data[0], data [1], data [2]);

	// do something only if button is pressed (but don't do anything if released)
	if (data [2] != 0) {
		// adjust volume: decrements until is reaches 0
		volume = (volume <= 0) ? 0 : (volume - 1);
		// set gain: 0 < gain < 1.0 (default = 0.2)
		fluid_settings_setnum (settings, "synth.gain", (float) volume/10.0f);

		// this part is useless as we cannot control the leds for now
		// if volume == 0, then light on volume down pad to indicate we have reached the lower limit
		if (volume == 0) {
				led (ctrl, ON);
				led (&fwd[1], OFF);		// this is a bit ugly
		}
		else {
			// if volume == 2 (default value), then light on both pads, in PENDING mode
			if (volume == 2) {
				led (ctrl, ON);
				led (&fwd[1], ON);		// this is a bit ugly
			}
			// in other cases, turn light of both pads (voldown and up)
			else {
				led (ctrl, OFF);
				led (&fwd[1], OFF);		// this is a bit ugly
			}
		}
	}

	// no need to update value of ctrl (it is not used)

	return FLUID_OK;
}

// process function called everytime fwd button is pressed and shift is ON
// MOMENTARY MODE ON
int process_fwd_shift (void *control, uint8_t *data)
{
	button_t *ctrl;
	ctrl = control;

//	printf ("FWD_SHIFT: %02X %02X %02X\n", data[0], data [1], data [2]);

	// do something only if button is pressed (but don't do anything if released)
	if (data [2] != 0) {
		// adjust volume: increments until is reaches 1
		volume = (volume >= 10) ? 10 : (volume + 1);
		// set gain: 0 < gain < 1.0 (default = 0.2)
		fluid_settings_setnum (settings, "synth.gain", (float) volume/10.0f);

		// this part is useless as we cannot control the leds for now
		// if volume == 10, then light on volume down pad to indicate we have reached the lower limit
		if (volume == 10) {
				led (&rwd[1], OFF);		// this is a bit ugly
				led (ctrl, ON);
		}
		else {
			// if volume == 2 (default value), then light on both pads, in PENDING mode
			if (volume == 2) {
				led (&rwd[1], ON);		// this is a bit ugly
				led (ctrl, ON);
			}
			// in other cases, turn light of both pads (voldown and up)
			else {
				led (&rwd[1], OFF);		// this is a bit ugly
				led (ctrl, OFF);
			}
		}
	}

	// no need to update value of ctrl (it is not used)

	return FLUID_OK;
}

// process function called everytime play button is pressed
// MOMENTARY MODE ON
int process_play (void *control, uint8_t *data)
{
	button_t *ctrl;
	ctrl = control;

//	printf ("PLAY: %02X %02X %02X\n", data[0], data [1], data [2]);

	// do something only if button is pressed (but don't do anything if released)
	if (data [2] != 0) {
		// load new midi file and new sf2, if required
		load_midi_sf2 ();
		
		// rewind to the beggining of the file
		fluid_player_seek (player, 0);
		// play the midi files, if any
		fluid_player_play (player);
	}

	// no need to update value of ctrl (it is not used)
	// no need to set any led (momentary mode ON)

	return FLUID_OK;
}

// process function called everytime stop button is pressed
// MOMENTARY MODE ON
int process_stop (void *control, uint8_t *data)
{
	button_t *ctrl;
	ctrl = control;

//	printf ("STOP: %02X %02X %02X\n", data[0], data [1], data [2]);

	// do something only if button is pressed (but don't do anything if released)
	if (data [2] != 0) {
		// stop playing the midi file, if any
		fluid_player_stop (player);
	}

	// no need to update value of ctrl (it is not used)
	// no need to set any led (momentary mode ON)

	return FLUID_OK;
}

// process function called everytime record button is pressed
// MOMENTARY MODE ON
int process_record (void *control, uint8_t *data)
{
	button_t *ctrl;
	ctrl = control;

//	printf ("RECORD: %02X %02X %02X\n", data[0], data [1], data [2]);

	// do something only if button is pressed (but don't do anything if released)
	if (data [2] != 0) {

		// make sure no file is playing to allow save !
		if ((fluid_player_get_status (player)== FLUID_PLAYER_DONE) || (fluid_player_get_status (player)== FLUID_PLAYER_READY)) save_song (new_midi_num);
	}

	// no need to update value of ctrl (it is not used)
	// no need to set any led (momentary mode ON)

	return FLUID_OK;
}

// process function called everytime set button is pressed
// MOMENTARY MODE ON
int process_set (void *control, uint8_t *data)
{
	int mark, i;
	button_t *ctrl;
	ctrl = control;

//	printf ("SET: %02X %02X %02X\n", data[0], data [1], data [2]);

	// do something only if button is pressed (but don't do anything if released)
	if (data [2] != 0) {
		// get current tick
		mark = fluid_player_get_current_tick (player);
		// save it to the table at the first available (ie. non-0) position
		// also make sure mark is not 0 (meaning : we are at the beginning of the file)
		if (mark != 0) {
			for (i = 0; i < NB_MARKER; i++) {
				if (marker [i] == 0) {
					marker [i] = mark;		// free slot found in marker table: store marker in table
printf ("set mark %d at tick %d\n", i, mark);
					break;					// and leave loop
				}
			}
		}
	}
	// no need to update value of ctrl (it is not used)
	// no need to set any led (no led for this control)

	return FLUID_OK;
}

// process function called everytime marker_l button is pressed
// MOMENTARY MODE ON
int process_marker_l (void *control, uint8_t *data)
{
	button_t *ctrl;
	ctrl = control;

//	printf ("MARKER_L: %02X %02X %02X\n", data[0], data [1], data [2]);

	// do something only if button is pressed (but don't do anything if released)
	if (data [2] != 0) {
		// leaves if no marker is set for current position
		if (marker [marker_pos] == 0) return FLUID_OK;

		// decrement position in marker table
		if (marker_pos > 0) marker_pos--;

		// seek position in the file set by the marker
		fluid_player_seek (player, marker [marker_pos]);
printf ("marker left, index %d tick %d\n", marker_pos, marker [marker_pos]);
	}

	// no need to update value of ctrl (it is not used)
	// no need to set any led (no led for this control)

	return FLUID_OK;
}

// process function called everytime marker_r button is pressed
// MOMENTARY MODE ON
int process_marker_r (void *control, uint8_t *data)
{
	button_t *ctrl;
	ctrl = control;

//	printf ("MARKER_R: %02X %02X %02X\n", data[0], data [1], data [2]);

	// do something only if button is pressed (but don't do anything if released)
	if (data [2] != 0) {
		// leaves if no marker is set for current position
		if (marker [marker_pos] == 0) return FLUID_OK;

		// increment position in marker table
		if (marker_pos < (NB_MARKER - 1)) marker_pos++;

		// in case there is no marker on new position, go back to old position
		if (marker [marker_pos] == 0) marker_pos--;

		// seek position in the file set by the marker
		fluid_player_seek (player, marker [marker_pos]);
printf ("marker right, index %d tick %d\n", marker_pos, marker [marker_pos]);
	}

	// no need to update value of ctrl (it is not used)
	// no need to set any led (no led for this control)

	return FLUID_OK;
}

// fluid callback called every time a MIDI message is received
int handle_midi_event(void* data, fluid_midi_event_t* event)
{
	fluid_synth_t* s;
	int i;
	channel_t *chan;			// intermediate struct to simplify code lisibility
	uint8_t mididata[3];		// one single structure regardless of midi event type
	
		// define data as being a pointer to fluid_synth_t structure
	s = (fluid_synth_t*) data;

	// fill mididata as per midi event type
	mididata[0] = fluid_midi_event_get_type(event);
	if ((mididata[0]==0x80) || (mididata[0]==0x90)) {
		// NOTE ON NOTE OFF
		mididata[1] = fluid_midi_event_get_key(event);
		mididata[2] = fluid_midi_event_get_velocity(event);
	}
	else {
		// other events like CC events
		// we won't manage program change, pitch bend, channel pressure, key pressure, midi system reset
		mididata[1] = fluid_midi_event_get_control(event);
		mididata[2] = fluid_midi_event_get_value(event);
	}

	// PLAY
	if (memcmp (play.message, mididata, 2)==0) return play.action (&play, mididata);
	// STOP
	if (memcmp (stop.message, mididata, 2)==0) return stop.action (&stop, mididata);
	// RECORD
	if (memcmp (record.message, mididata, 2)==0) return record.action (&record, mididata);

	// SET
	if (memcmp (set.message, mididata, 2)==0) return set.action (&set, mididata);
	// MARKER_L
	if (memcmp (marker_l.message, mididata, 2)==0) return marker_l.action (&marker_l, mididata);
	// MARKER_R
	if (memcmp (marker_r.message, mididata, 2)==0) return marker_r.action (&marker_r, mididata);

	// CYCLE
	if (memcmp (cycle.message, mididata, 2)==0) return cycle.action (&cycle, mididata);

	// TRACK_L
	if (memcmp (track_l[cycle.value].message, mididata, 2)==0) return track_l[cycle.value].action (&track_l[cycle.value], mididata);
	// TRACK_R
	if (memcmp (track_r[cycle.value].message, mididata, 2)==0) return track_r[cycle.value].action (&track_r[cycle.value], mididata);
	// RWD
	if (memcmp (rwd[cycle.value].message, mididata, 2)==0) return rwd[cycle.value].action (&rwd[cycle.value], mididata);
	// FWD
	if (memcmp (fwd[cycle.value].message, mididata, 2)==0) return fwd[cycle.value].action (&fwd[cycle.value], mididata);

	// CHANNELS
	// check whether received event correponds to a channel event
	for (i = 0; i<NB_CHANNEL; i++) {
		chan = & (channel[i][channel[i][0].rec.value & 0x01]);
		// SLIDER
		if (memcmp (chan->slider.message, mididata, 2)==0) return chan->slider.action (&(chan->slider), mididata);
		// KNOB
		if (memcmp (chan->knob.message, mididata, 2)==0) return chan->knob.action (&(chan->knob), mididata);
		// SOLO
		if (memcmp (chan->solo.message, mididata, 2)==0) return chan->solo.action (&(chan->solo), mididata);
		// MUTE
		if (memcmp (chan->mute.message, mididata, 2)==0) return chan->mute.action (&(chan->mute), mididata);
		// REC: take only value 0 into account
		chan = & (channel[i][0]);
		if (memcmp (chan->rec.message, mididata, 2)==0) return chan->rec.action (&(chan->rec), mididata);
	}

//	fluid_synth_handle_midi_event((fluid_synth_t*) data, event);

	return FLUID_OK;
}

