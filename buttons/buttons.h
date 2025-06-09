#ifndef my_buttons_h
#define my_buttons_h 1

#include "pico/stdlib.h"

#define BUTTON_U 9
#define BUTTON_D 10
#define BUTTON_L 11
#define BUTTON_R 12
#define BUTTON_M 13

/* Initialisiert gpio */
void buttons_init();

/* Gibt den aktuell gedrueckten Button zurück oder den Wert 100 */
uint8_t get_Button();

#endif
