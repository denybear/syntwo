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
int process (void *ctrl, uint8_t *data)
{
	printf ("in PROCESS function: %02X %02X %02X\n", data[0], data [1], data [2]);

	return FLUID_OK;
}

// process function called everytime slide is actionned
int process_slider (slider_t *ctrl, uint8_t *data)
{
	printf ("SLIDER: %02X %02X %02X\n", data[0], data [1], data [2]);

	// get value from the midi control
	ctrl->value = data[2];
	// send CC7 (sound control) to synthetizer
	// data[1] is the channel number
	fluid_synth_cc (data[1], 7, ctrl->value);

	return FLUID_OK;
}

// process function called everytime slide is actionned for channels > 8
int process_slider_shift (slider_t *ctrl, uint8_t *data)
{
	printf ("SLIDER_SHIFT: %02X %02X %02X\n", data[0], data [1], data [2]);

	// get value from the midi control
	ctrl->value = data[2];
	// send CC7 (sound control) to synthetizer
	// data[1] is the channel number
	fluid_synth_cc (data[1]+0x08, 7, ctrl->value);

	return FLUID_OK;
}

// process function called everytime knob is actionned
int process_knob (knob_t *ctrl, uint8_t *data)
{
	printf ("KNOB: %02X %02X %02X\n", data[0], data [1], data [2]);

	// get value from the midi control
	ctrl->value = data[2];
	// send CC8 (balance) to synthetizer
	// data[1] is the channel number
	fluid_synth_cc (data[1]-0x10, 8, ctrl->value);

	return FLUID_OK;
}

// process function called everytime knob is actionned for channels > 8
int process_knob_shift (knob_t *ctrl, uint8_t *data)
{
	printf ("KNOB_SHIFT: %02X %02X %02X\n", data[0], data [1], data [2]);

	// get value from the midi control
	ctrl->value = data[2];
	// send CC8 (balance) to synthetizer
	// data[1] is the channel number
	fluid_synth_cc (data[1]-0x10+0x08, 8, ctrl->value);

	return FLUID_OK;
}


// process function called everytime cycle button is pressed
// TOGGLE MODE ON
int process_cycle (button_t *ctrl, uint8_t *data)
{
	printf ("CYCLE: %02X %02X %02X\n", data[0], data [1], data [2]);

	// get value from the midi control: 0 or 1 (OFF or ON)
	ctrl->value = data[2] & 0x01;		// &0x01 as value in midi message is 7F in case of ON
	// switch led on/off accordingly
	led (ctrl, ctrl->value);

	return FLUID_OK;
}

// process function called everytime track_l button is pressed
// MOMENTARY MODE ON
int process_track_l (button_t *ctrl, uint8_t *data)
{
	printf ("TRACK_L: %02X %02X %02X\n", data[0], data [1], data [2]);

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
int process_track_r (button_t *ctrl, uint8_t *data)
{
	printf ("TRACK_R: %02X %02X %02X\n", data[0], data [1], data [2]);

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
int process_track_l_shift (button_t *ctrl, uint8_t *data)
{
	printf ("TRACK_L_SHIFT: %02X %02X %02X\n", data[0], data [1], data [2]);

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
int process_track_r_shift (button_t *ctrl, uint8_t *data)
{
	printf ("TRACK_R_SHIFT: %02X %02X %02X\n", data[0], data [1], data [2]);

	// do something only if button is pressed (but don't do anything if released)
	if (data [2] != 0) {
		// increase soundfont file number until it reaches FF
		if (new_sf2_num < 0xFF) new_sf2_num++; 
	}

	// no need to update value of ctrl (it is not used)
	// no need to set any led (there are no led for this key)

	return FLUID_OK;
}

// process function called everytime rwd button is pressed
// MOMENTARY MODE ON
int process_rwd (button_t *ctrl, uint8_t *data)
{
	printf ("RWD: %02X %02X %02X\n", data[0], data [1], data [2]);

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

// process function called everytime fwd button is pressed
// MOMENTARY MODE ON
int process_fwd (button_t *ctrl, uint8_t *data)
{
	printf ("FWD: %02X %02X %02X\n", data[0], data [1], data [2]);

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
int process_rwd_shift (button_t *ctrl, uint8_t *data)
{
	printf ("RWD_SHIFT: %02X %02X %02X\n", data[0], data [1], data [2]);

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
int process_fwd_shift (button_t *ctrl, uint8_t *data)
{
	printf ("FWD_SHIFT: %02X %02X %02X\n", data[0], data [1], data [2]);

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
int process_play (button_t *ctrl, uint8_t *data)
{
	printf ("PLAY: %02X %02X %02X\n", data[0], data [1], data [2]);

	// do something only if button is pressed (but don't do anything if released)
	if (data [2] != 0) {
		// load new midi file and new sf2, if required
		// SHOULD WE DO THIS IN THE MAIN THREAD???
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
int process_stop (button_t *ctrl, uint8_t *data)
{
	printf ("STOP: %02X %02X %02X\n", data[0], data [1], data [2]);

	// do something only if button is pressed (but don't do anything if released)
	if (data [2] != 0) {
		// stop playing the midi file, if any
		fluid_player_stop (player);
	}

	// no need to update value of ctrl (it is not used)
	// no need to set any led (momentary mode ON)

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
	if (memcmp (play.message, mididata, 2)) return play.action (&play, mididata);
	// STOP
	if (memcmp (stop.message, mididata, 2)) return stop.action (&stop, mididata);
	// RECORD
	if (memcmp (record.message, mididata, 2)) return record.action (&record, mididata);

	// SET
	if (memcmp (set.message, mididata, 2)) return set.action (&set, mididata);
	// MARKER_L
	if (memcmp (marker_l.message, mididata, 2)) return marker_l.action (&marker_l, mididata);
	// MARKER_R
	if (memcmp (marker_r.message, mididata, 2)) return marker_r.action (&marker_r, mididata);

	// CYCLE
	if (memcmp (cycle.message, mididata, 2)) return cycle.action (&cycle, mididata);

	// TRACK_L
	if (memcmp (track_l[cycle.value].message, mididata, 2)) return track_l[cycle.value].action (&track_l[cycle.value], mididata);
	// TRACK_R
	if (memcmp (track_r[cycle.value].message, mididata, 2)) return track_r[cycle.value].action (&track_r[cycle.value], mididata);
	// RWD
	if (memcmp (rwd[cycle.value].message, mididata, 2)) return rwd[cycle.value].action (&rwd[cycle.value], mididata);
	// FWD
	if (memcmp (fwd[cycle.value].message, mididata, 2)) return fwd[cycle.value].action (&fwd[cycle.value], mididata);

	// CHANNELS
	// check whether received event correponds to a channel event
	for (i = 0; i<NB_CHANNEL; i++) {
		chan = & (channel[i][channel[i][0].rec.value]);
		// SLIDER
		if (memcmp (chan->slider.message, mididata, 2)) return chan->slider.action (&(chan->slider), mididata);
		// KNOB
		if (memcmp (chan->knob.message, mididata, 2)) return chan->knob.action (&(chan->knob), mididata);
		// SOLO
		if (memcmp (chan->solo.message, mididata, 2)) return chan->solo.action (&(chan->solo), mididata);
		// MUTE
		if (memcmp (chan->mute.message, mididata, 2)) return chan->mute.action (&(chan->mute), mididata);
		// REC: take only value 0 into account
		chan = & (channel[i][0]);
		if (memcmp (chan->rec.message, mididata, 2)) return chan->rec.action (&(chan->rec), mididata);
	}

//	fluid_synth_handle_midi_event((fluid_synth_t*) data, event);

	return FLUID_OK;
}

