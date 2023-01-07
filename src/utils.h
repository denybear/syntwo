/** @file utils.h
 *
 * @brief This file defines prototypes of functions inside utils.c
 *
 */

int get_full_filename (char *, unsigned char, char *);
int load_midi_sf2 ();
uint64_t micros ();
void led (button_t *, int);
