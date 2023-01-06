/** @file process.h
 *
 * @brief This file defines prototypes of functions inside process.c
 *
 */

int process (uint8_t*);
int handle_midi_event(void*, fluid_midi_event_t*);
int gpio_process ();
int beat_process ();


