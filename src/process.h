/** @file process.h
 *
 * @brief This file defines prototypes of functions inside process.c
 *
 */

int process (void*, uint8_t*);
int process_slider (slider_t *, uint8_t *)
int process_slider_shift (slider_t *, uint8_t *)
int process_knob (knob_t *, uint8_t *)
int process_knob_shift (knob_t *, uint8_t *)
int process_solo (button_t *, uint8_t *);
int process_solo_shift (button_t *, uint8_t *);
int process_mute (button_t *, uint8_t *);
int process_mute_shift (button_t *, uint8_t *);
int process_rec (button_t *, uint8_t *);
int process_cycle (button_t *, uint8_t *);
int process_track_l (button_t *, uint8_t *);
int process_track_r (button_t *, uint8_t *);
int process_track_l_shift (button_t *, uint8_t *);
int process_track_r_shift (button_t *, uint8_t *);
int process_rwd (button_t *, uint8_t *);
int process_fwd (button_t *, uint8_t *);
int process_rwd_shift (button_t *, uint8_t *);
int process_fwd_shift (button_t *, uint8_t *);
int process_play (button_t *, uint8_t *);
int process_stop (button_t *, uint8_t *);
int handle_midi_event(void*, fluid_midi_event_t*);


