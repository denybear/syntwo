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


/* This example reads the configuration file 'example.cfg' and displays
 * some of its contents.
 */

void read_config (void)
{
	uint8_t i,j;
	
	// set channels 
	for (i = 0; i<NB_CHANNEL; i++) {
		for (j = 0; j<NB_RECSHIFT; j++) {
			// CC7 volume control
			channel[i][j].slider.message [0] = 0xB0 + (i * (j+1));
			channel[i][j].slider.message [1] = 0x07;
			channel[i][j].slider.action = &process;

			// CC10 balance control
			channel[i][j].knob.message [0] = 0xB0 + (i * (j+1));
			channel[i][j].knob.message [1] = 0x0A;
			channel[i][j].knob.action = &process;

			// solo button
			channel[i][j].solo.message [0] = 0x90;
			channel[i][j].solo.message [1] = 0x10 + (i * (j+1));
			channel[i][j].solo.action = &process;
			channel[i][j].solo.led_on [0] = 0x90;
			channel[i][j].solo.led_on [1] = 0x10 + (i * (j+1));
			channel[i][j].solo.led_on [2] = 0x7F;
			channel[i][j].solo.led_off [0] = 0x90;
			channel[i][j].solo.led_off [1] = 0x10 + (i * (j+1));
			channel[i][j].solo.led_off [2] = 0x00;

			// mute button
			channel[i][j].mute.message [0] = 0x90;
			channel[i][j].mute.message [1] = 0x20 + (i * (j+1));
			channel[i][j].mute.action = &process;
			channel[i][j].mute.led_on [0] = 0x90;
			channel[i][j].mute.led_on [1] = 0x20 + (i * (j+1));
			channel[i][j].mute.led_on [2] = 0x7F;
			channel[i][j].mute.led_off [0] = 0x90;
			channel[i][j].mute.led_off [1] = 0x20 + (i * (j+1));
			channel[i][j].mute.led_off [2] = 0x00;

			// rec button
			channel[i][j].rec.message [0] = 0x90;
			channel[i][j].rec.message [1] = 0x30 + (i * (j+1));
			channel[i][j].rec.action = &process;
			channel[i][j].rec.led_on [0] = 0x90;
			channel[i][j].rec.led_on [1] = 0x30 + (i * (j+1));
			channel[i][j].rec.led_on [2] = 0x7F;
			channel[i][j].rec.led_off [0] = 0x90;
			channel[i][j].rec.led_off [1] = 0x30 + (i * (j+1));
			channel[i][j].rec.led_off [2] = 0x00;
		}
	}

	// set tracks and rwd/ffd
	for (i = 0; i<NB_CYCSHIFT; i++) {
		// track_l button
		track_l[i].message [0] = 0x90;
		track_l[i].message [1] = 0x40;
		track_l[i].action = &process;
		track_l[i].led_on [0] = 0x90;
		track_l[i].led_on [1] = 0x40;
		track_l[i].led_on [2] = 0x7F;
		track_l[i].led_off [0] = 0x90;
		track_l[i].led_off [1] = 0x40;
		track_l[i].led_off [2] = 0x00;

		// track_r button
		track_r[i].message [0] = 0x90;
		track_r[i].message [1] = 0x41;
		track_r[i].action = &process;
		track_r[i].led_on [0] = 0x90;
		track_r[i].led_on [1] = 0x41;
		track_r[i].led_on [2] = 0x7F;
		track_r[i].led_off [0] = 0x90;
		track_r[i].led_off [1] = 0x41;
		track_r[i].led_off [2] = 0x00;

		// rewind button
		rwd[i].message [0] = 0x90;
		rwd[i].message [1] = 0x42;
		rwd[i].action = &process;
		rwd[i].led_on [0] = 0x90;
		rwd[i].led_on [1] = 0x42;
		rwd[i].led_on [2] = 0x7F;
		rwd[i].led_off [0] = 0x90;
		rwd[i].led_off [1] = 0x42;
		rwd[i].led_off [2] = 0x00;

		// forward button
		fwd[i].message [0] = 0x90;
		fwd[i].message [1] = 0x43;
		fwd[i].action = &process;
		fwd[i].led_on [0] = 0x90;
		fwd[i].led_on [1] = 0x43;
		fwd[i].led_on [2] = 0x7F;
		fwd[i].led_off [0] = 0x90;
		fwd[i].led_off [1] = 0x43;
		fwd[i].led_off [2] = 0x00;
	}

	// cycle button
	cycle.message [0] = 0x90;
	cycle.message [1] = 0x50;
	cycle.action = &process;
	cycle.led_on [0] = 0x90;
	cycle.led_on [1] = 0x50;
	cycle.led_on [2] = 0x7F;
	cycle.led_off [0] = 0x90;
	cycle.led_off [1] = 0x50;
	cycle.led_off [2] = 0x00;

	// play button
	play.message [0] = 0x90;
	play.message [1] = 0x51;
	play.action = &process;
	play.led_on [0] = 0x90;
	play.led_on [1] = 0x51;
	play.led_on [2] = 0x7F;
	play.led_off [0] = 0x90;
	play.led_off [1] = 0x51;
	play.led_off [2] = 0x00;

	// stop button
	stop.message [0] = 0x90;
	stop.message [1] = 0x52;
	stop.action = &process;
	stop.led_on [0] = 0x90;
	stop.led_on [1] = 0x52;
	stop.led_on [2] = 0x7F;
	stop.led_off [0] = 0x90;
	stop.led_off [1] = 0x52;
	stop.led_off [2] = 0x00;

	// record button
	record.message [0] = 0x90;
	record.message [1] = 0x53;
	record.action = &process;
	record.led_on [0] = 0x90;
	record.led_on [1] = 0x53;
	record.led_on [2] = 0x7F;
	record.led_off [0] = 0x90;
	record.led_off [1] = 0x53;
	record.led_off [2] = 0x00;

	// set button
	set.message [0] = 0x90;
	set.message [1] = 0x54;
	set.action = &process;
	set.led_on [0] = 0x90;
	set.led_on [1] = 0x54;
	set.led_on [2] = 0x7F;
	set.led_off [0] = 0x90;
	set.led_off [1] = 0x54;
	set.led_off [2] = 0x00;

	// marker_l button
	marker_l.message [0] = 0x90;
	marker_l.message [1] = 0x55;
	marker_l.action = &process;
	marker_l.led_on [0] = 0x90;
	marker_l.led_on [1] = 0x55;
	marker_l.led_on [2] = 0x7F;
	marker_l.led_off [0] = 0x90;
	marker_l.led_off [1] = 0x55;
	marker_l.led_off [2] = 0x00;

	// marker_r button
	marker_r.message [0] = 0x90;
	marker_r.message [1] = 0x56;
	marker_r.action = &process;
	marker_r.led_on [0] = 0x90;
	marker_r.led_on [1] = 0x56;
	marker_r.led_on [2] = 0x7F;
	marker_r.led_off [0] = 0x90;
	marker_r.led_off [1] = 0x56;
	marker_r.led_off [2] = 0x00;
}
