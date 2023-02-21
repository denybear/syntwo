/** @file utils.h
 *
 * @brief This file defines prototypes of functions inside utils.c
 *
 */

int get_full_filename (char *, unsigned char, char *);
int load_midi_sf2 ();
uint64_t micros ();
void led (button_t *, int);
int save_song (int);
int load_song (int);
int set_slider_value (uint8_t);
int set_volume_value (uint8_t);
int reset_song_volume ();
int set_knob_value (uint8_t);
int set_balance_value (uint8_t);
int reset_song_balance ();
