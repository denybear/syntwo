/** @file process.h
 *
 * @brief This file defines prototypes of functions inside process.c
 *
 */

int handle_midi_event(void*, fluid_midi_event_t*);
int process ( jack_nframes_t, void *);
int midi_in_process (jack_midi_event_t *, jack_nframes_t);
int gpio_process ();
int beat_process ();
int handle_tick(void *, int);

