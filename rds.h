// Typedefinitionen des RDS
#ifndef my_dsa_h
#define my_rds_h 1

#include "pico/stdlib.h"
char PTY[32][9] = {
	"        ", "News    ", "Aktuell ", "Info    ",
	"Sport   ", "Bildung ", "Drama   ", "Kultur  ",
	"Wissen  ", "Variabel", "Pop     ", "Rock    ",
	"Leicht  ", "Klassik1", "Klassik2", "Anderes ",
	"Wetter  ", "Geld    ", "Kinder  ", "Soziales",
	"Religion", "Anrufer ", "Reisen  ", "Freizeit",
	"Jazz    ", "Country ", "National", "Oldies  ",
	"Folk    ", "Doku    ", "AlarmTst", "Alarm   "
};

/* see page 12 in ISO 14819-1 */
char tmc_duration[8][2][23]={
	{"no duration given", "no duration given"},
	{"15 minutes", "next few hours"},
	{"30 minutes", "rest of the day"},
	{"1 hour", "until tomorrow evening"},
	{"2 hours", "rest of the week"},
	{"3 hours", "end of next week"},
	{"4 hours", "end of the month"},
	{"rest of the day", "long period"}
};

char label_descriptions[16][32]={
	"Duration",
	"Control code",
	"Length of route affected",
	"Speed limit advice",
	"Quantifier",
	"Quantifier",
	"Supplementary information code",
	"Explicit start time",
	"Explicit stop time",
	"Additional event",
	"Detailed diversion instructions",
	"Destination",
	"RFU (Reserved for future use)",
	"Cross linkage to source of probl",
	"Separator",
	"RFU (Reserved for future use)"
};

uint8_t optional_content_lengths[16]={3,3,5,5,5,8,8,8,8,11,16,16,16,16,0,0};
#endif
