/** @file process.h
 *
 * @brief This file defines prototypes of functions inside process.c
 *
 */

int process (void *, uint8_t*);
int process_slider (void *, uint8_t *);
int process_slider_shift (void *, uint8_t *);
int process_knob (void *, uint8_t *);
int process_knob_shift (void *, uint8_t *);
int process_solo (void *, uint8_t *);
int process_solo_shift (void *, uint8_t *);
int process_mute (void *, uint8_t *);
int process_mute_shift (void *, uint8_t *);
int process_rec (void *, uint8_t *);
int process_cycle (void *, uint8_t *);
int process_track_l (void *, uint8_t *);
int process_track_r (void *, uint8_t *);
int process_track_l_shift (void *, uint8_t *);
int process_track_r_shift (void *, uint8_t *);
int process_rwd (void *, uint8_t *);
int process_fwd (void *, uint8_t *);
int process_rwd_shift (void *, uint8_t *);
int process_fwd_shift (void *, uint8_t *);
int process_play (void *, uint8_t *);
int process_stop (void *, uint8_t *);
int process_record (void *, uint8_t *);
int process_set (void *, uint8_t *);
int process_marker_l (void *, uint8_t *);
int process_marker_r (void *, uint8_t *);
int handle_midi_event_from_hw (void*, fluid_midi_event_t*);
int handle_midi_event_to_synth (void*, fluid_midi_event_t*);
uint8_t adjust_volume (uint8_t, uint8_t); 
uint8_t adjust_panning (uint8_t, uint8_t);

