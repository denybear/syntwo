/** @file config.c
 *
 * @brief The config module assigns Korg midi kontrol midi codes to enable control and led on/off
 *
 */

#include "types.h"
#include "globals.h"
#include "config.h"
#include "process.h"
#include "utils.h"
#include "gpio.h"


/* This example reads the configuration file 'example.cfg' and displays
 * some of its contents.
 */

void read_config (void)
{
	uint8_t i,j;
	
	// note: "CHANNEL" nibble in byte 1 of MIDI message is NOT taken into account
	// set channels 
	// for KORG NANOKONTROL 2: settings should be CC, toggle mode ON for CYCLE, SOLO, MUTE, REC

	for (i = 0; i<NB_CHANNEL; i++) {
		for (j = 0; j<NB_RECSHIFT; j++) {
			// CC7 volume control
			channel[i][j].slider.message [0] = 0xB0;
			channel[i][j].slider.message [1] = i;
			channel[i][j].slider.action = &process_slider;

			// CC10 balance control
			channel[i][j].knob.message [0] = 0xB0;
			channel[i][j].knob.message [1] = 0x10 + i;
			channel[i][j].knob.action = &process_knob;

			// solo button
			channel[i][j].solo.message [0] = 0xB0;
			channel[i][j].solo.message [1] = 0x20 + i;
			channel[i][j].solo.action = &process;
			channel[i][j].solo.led_on [0] = 0xB0;
			channel[i][j].solo.led_on [1] = 0x20 + i;
			channel[i][j].solo.led_on [2] = 0x7F;
			channel[i][j].solo.led_off [0] = 0xB0;
			channel[i][j].solo.led_off [1] = 0x20 + i;
			channel[i][j].solo.led_off [2] = 0x00;

			// mute button
			channel[i][j].mute.message [0] = 0xB0;
			channel[i][j].mute.message [1] = 0x30 + i;
			channel[i][j].mute.action = &process;
			channel[i][j].mute.led_on [0] = 0xB0;
			channel[i][j].mute.led_on [1] = 0x30 + i;
			channel[i][j].mute.led_on [2] = 0x7F;
			channel[i][j].mute.led_off [0] = 0xB0;
			channel[i][j].mute.led_off [1] = 0x30 + i;
			channel[i][j].mute.led_off [2] = 0x00;

			// rec button
			channel[i][j].rec.message [0] = 0xB0;
			channel[i][j].rec.message [1] = 0x40 + i;
			channel[i][j].rec.action = &process;
			channel[i][j].rec.led_on [0] = 0xB0;
			channel[i][j].rec.led_on [1] = 0x40 + i;
			channel[i][j].rec.led_on [2] = 0x7F;
			channel[i][j].rec.led_off [0] = 0xB0;
			channel[i][j].rec.led_off [1] = 0x40 + i;
			channel[i][j].rec.led_off [2] = 0x00;
		}
	}

	// set tracks and rwd/fwd
	for (i = 0; i<NB_CYCSHIFT; i++) {
		// track_l button
		track_l[i].message [0] = 0xB0;
		track_l[i].message [1] = 0x3A;
		if (i==0) track_l[i].action = &process_track_l;
		else track_l[i].action = &process_track_l_shift;
		track_l[i].led_on [0] = 0xB0;
		track_l[i].led_on [1] = 0x3A;
		track_l[i].led_on [2] = 0x7F;
		track_l[i].led_off [0] = 0xB0;
		track_l[i].led_off [1] = 0x3A;
		track_l[i].led_off [2] = 0x00;

		// track_r button
		track_r[i].message [0] = 0xB0;
		track_r[i].message [1] = 0x3B;
		if (i==0) track_r[i].action = &process_track_r;
		else track_r[i].action = &process_track_r_shift;
		track_r[i].led_on [0] = 0xB0;
		track_r[i].led_on [1] = 0x3B;
		track_r[i].led_on [2] = 0x7F;
		track_r[i].led_off [0] = 0xB0;
		track_r[i].led_off [1] = 0x3B;
		track_r[i].led_off [2] = 0x00;

		// rewind button
		rwd[i].message [0] = 0xB0;
		rwd[i].message [1] = 0x2B;
		if (i==0) rwd[i].action = &process_rwd;
		else rwd[i].action = &process_rwd_shift;
		rwd[i].led_on [0] = 0xB0;
		rwd[i].led_on [1] = 0x2B;
		rwd[i].led_on [2] = 0x7F;
		rwd[i].led_off [0] = 0xB0;
		rwd[i].led_off [1] = 0x2B;
		rwd[i].led_off [2] = 0x00;

		// forward button
		fwd[i].message [0] = 0xB0;
		fwd[i].message [1] = 0x2C;
		if (i==0) fwd[i].action = &process_fwd;
		else fwd[i].action = &process_fwd_shift;
		fwd[i].led_on [0] = 0xB0;
		fwd[i].led_on [1] = 0x2C;
		fwd[i].led_on [2] = 0x7F;
		fwd[i].led_off [0] = 0xB0;
		fwd[i].led_off [1] = 0x2C;
		fwd[i].led_off [2] = 0x00;
	}

	// cycle button
	cycle.message [0] = 0xB0;
	cycle.message [1] = 0x2E;
	cycle.action = &process_cycle;
	cycle.led_on [0] = 0xB0;
	cycle.led_on [1] = 0x2E;
	cycle.led_on [2] = 0x7F;
	cycle.led_off [0] = 0xB0;
	cycle.led_off [1] = 0x2E;
	cycle.led_off [2] = 0x00;

	// play button
	play.message [0] = 0xB0;
	play.message [1] = 0x29;
	play.action = &process_play;
	play.led_on [0] = 0xB0;
	play.led_on [1] = 0x29;
	play.led_on [2] = 0x7F;
	play.led_off [0] = 0xB0;
	play.led_off [1] = 0x29;
	play.led_off [2] = 0x00;

	// stop button
	stop.message [0] = 0xB0;
	stop.message [1] = 0x2A;
	stop.action = &process_stop;
	stop.led_on [0] = 0xB0;
	stop.led_on [1] = 0x2A;
	stop.led_on [2] = 0x7F;
	stop.led_off [0] = 0xB0;
	stop.led_off [1] = 0x2A;
	stop.led_off [2] = 0x00;

	// record button
	record.message [0] = 0xB0;
	record.message [1] = 0x2D;
	record.action = &process;
	record.led_on [0] = 0xB0;
	record.led_on [1] = 0x2D;
	record.led_on [2] = 0x7F;
	record.led_off [0] = 0xB0;
	record.led_off [1] = 0x2D;
	record.led_off [2] = 0x00;

	// set button
	set.message [0] = 0xB0;
	set.message [1] = 0x3C;
	set.action = &process;
	set.led_on [0] = 0xB0;
	set.led_on [1] = 0x3C;
	set.led_on [2] = 0x7F;
	set.led_off [0] = 0xB0;
	set.led_off [1] = 0x3C;
	set.led_off [2] = 0x00;

	// marker_l button
	marker_l.message [0] = 0xB0;
	marker_l.message [1] = 0x3D;
	marker_l.action = &process;
	marker_l.led_on [0] = 0xB0;
	marker_l.led_on [1] = 0x3D;
	marker_l.led_on [2] = 0x7F;
	marker_l.led_off [0] = 0xB0;
	marker_l.led_off [1] = 0x3D;
	marker_l.led_off [2] = 0x00;

	// marker_r button
	marker_r.message [0] = 0xB0;
	marker_r.message [1] = 0x3E;
	marker_r.action = &process;
	marker_r.led_on [0] = 0xB0;
	marker_r.led_on [1] = 0x3E;
	marker_r.led_on [2] = 0x7F;
	marker_r.led_off [0] = 0xB0;
	marker_r.led_off [1] = 0x3E;
	marker_r.led_off [2] = 0x00;
}
