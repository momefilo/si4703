// radio
#include "ili_9341/ili_9341.h"
#include "si4703/si4703.h"
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/double.h"

uint8_t *Register;
// Helferfunktionen fuer das Userinterface
char Menu[31][8] = {
	"DSMUTE",//	0 bool
	"DMUTE",//	1 bool
	"MONO",//	2 bool
	"RDSM",// 	3 bool
	"SKMODE",//	4 bool
	"RDSIEN",// 5 bool
	"STCIEN",// 6 bool
	"RDS",// 	7 bool
	"DE",// 	8 bool
	"AGCD",// 	9 bool
	"TUNE",// 	10 bool
	"AHIZEN",// 11 bool
	"VOLEXT",// 12 bool;
	"RSDPRF",// 13 bool
	"XOSCEN",// 14 bool
	"BAND  ",//15 2bit
	"SPACE",// 	16 2bit
	"BLNDADJ",//17 2bit
	"SEEKTH",// 18 8bit
	"SKSNR",// 	19 4bit
	"SKCNT",// 	20 4bit
	"SMUTER",// 21 2bit
	"SMUTEA",// 22 2bit
	"GP3",// 	23 2bit
	"GP2",// 	24 2bit
	"GP1",// 	25 2bit
	"CHAN",//	26 10bit
	"ENABLE",// 27 bool
	"VOLUME",// 28 4bit
	"SEEKUP",// 29 bool
	"SEEK",// 	30 bool
};
char Status[16][8] = {
	"RDSR",//	0 bool
	"STC",// 	1 bool
	"SF/BL",//  2 bool
	"AFCRL",// 	3 bool
	"RDSS",// 	4 bool
	"ST",// 	5 bool
	"RSSI",// 	6 8 bit
	"BLERA",// 	7 2 Bit
	"BLERB",// 	8 2 bit
	"BLERC",// 	9 2 bit
	"BLERD",// 	10 2 bit
	"CHAN",// 	11 10 bit
	"RDSA",// 	12 16bit
	"RDSB",// 	13 16bit
	"RDSC",//  	14 16bit
	"RDSD",// 	15 16bit
};
char *RegText[8][4] = {
	{"87-108", "76-108", "76-90", "res"},// 		15 BAND
	{"200", "100", "50", "res"},//					16 SPACE
	{"31-49", "37-55", "19-37", "25-43"},//	17 BLNDADJ
	{"blitz", "fast", "norm", "slow"},//	21 SMUTER
	{"16 db", "14 db", "12 db", "10 db"},//	22 SMUTEA
	{"HiI", "M/S", "Low", "Hig"},//			23 GP3
	{"HiI", "IrE", "Low", "Hig"},//			24 GP2
	{"HiI", "res", "Low", "Hig"}//			25 GP1
};
char **getHumanRead(uint8_t nr){
	switch(nr){
		case 15: return RegText[0]; break;
		case 16: return RegText[1]; break;
		case 17: return RegText[2]; break;
		case 21: return RegText[3]; break;
		case 22: return RegText[4]; break;
		case 23: return RegText[5]; break;
		case 24: return RegText[6]; break;
		case 25: return RegText[7]; break;
	}
}
uint16_t getStatusReg(uint8_t nr){
	switch(nr){
		case 0: return Register[16] & 0x80; break;// RDSR
		case 1: return Register[16] & 0x40; break;// STC
		case 2: return Register[16] & 0x20; break;// SF/BL
		case 3: return Register[16] & 0x10; break;// AFCRL
		case 4: return Register[16] & 0x08; break;//RDSS
		case 5: return Register[16] & 0x01; break;//ST
		case 6: return Register[17]; break;//RSSI
		case 7: return Register[16] & 0x06; break;//BLERA
		case 8: return Register[18] & 0xC0; break;//BLERB
		case 9: return Register[18] & 0x30; break;//BLERC
		case 10: return Register[18] & 0x0C; break;//BLERD
		case 11: return (Register[19] | ((Register[18] & 0x03)<<8)); break;//READCHAN
		case 12: return (Register[21] | ((Register[20])<<8)); break;//RDSA
		case 13: return (Register[23] | ((Register[22])<<8)); break;//RDSB
		case 14: return (Register[25] | ((Register[24])<<8)); break;//RDSC
		case 15: return (Register[27] | ((Register[26])<<8)); break;//RDSD
	}
}
uint16_t getMenuReg(uint8_t nr){
	switch(nr){
		case 0: return Register[0] & 0x80; break;//DSMUTE
		case 1: return Register[0] & 0x40; break;//DMUTE
		case 2: return Register[0] & 0x20; break;//MONO
		case 3: return Register[0] & 0x08; break;//RDSM
		case 4: return Register[0] & 0x04; break;//SEEKMODE
		case 5: return Register[4] & 0x80; break;//RDSEN
		case 6: return Register[4] & 0x40; break;//STCEN
		case 7: return Register[4] & 0x10; break;//RDS
		case 8: return Register[4] & 0x08; break;//DE
		case 9: return Register[4] & 0x04; break;//AGCD
		case 10: return Register[2] & 0x80; break;//TUNE
		case 11: return Register[10] & 0x40; break;//AHIZEN
		case 12: return Register[8] & 0x01; break;//VOLEXT
		case 13: return Register[8] & 0x02; break;//SDRPRF
		case 14: return Register[10] & 0x80; break;//XOSCEN
		case 15: return Register[7] & 0xC0; break;//BAND
		case 16: return Register[7] & 0x30; break;//SPACE
		case 17: return Register[5] & 0xC0; break;//BLNDADJ
		case 18: return Register[6]; break;//SEEKTH
		case 19: return Register[9] & 0xF0; break;//SKSNR
		case 20: return Register[9] & 0x0F; break;//SKCNT
		case 21: return Register[8] & 0xC0; break;//SMUTER
		case 22: return Register[8] & 0x30; break;//SMUTEA
		case 23: return Register[5] & 0x30; break;//GP3
		case 24: return Register[5] & 0x0C; break;//GP2
		case 25: return Register[5] & 0x03; break;//GP1
		case 26: return Register[3]; break;//CHAN
		case 27: return Register[1] & 0x01; break;//ENABLE
		case 28: return Register[7] & 0x0F; break;//VOLUME
		case 29: return Register[0] & 0x02; break;//SEEKUP
		case 30: return Register[0] & 0x01; break;//SEEK
	}
}
char *getRegText(uint8_t nr){ return getHumanRead(nr)[getMenuReg(nr)]; }
float_t get_mhz(){
	uint16_t chan = getStatusReg(11);
	float_t space = 0.2;
	if(Register[7] & 0x10) space = 0.1;
	if(Register[7] & 0x20) space = 0.05;
	float_t ofset = 76.0;
	if( (Register[7] & 0xC0) == 0) ofset = 87.5;
	return chan * space + ofset;
}

// Das Userinterface
void paint_rssi(){
	uint8_t top = 51;// Startzeile
	uint16_t area0[] = {115, top, 219, top+16};
	paintRect(area0, 0x0000);
	area0[3] = top;
	paintRect(area0, 0xFFE0);
	area0[2] = 115; area0[3] = top+16;
	paintRect(area0, 0xFFE0);
	area0[2] = 219; area0[1] = top+16;
	paintRect(area0, 0xFFE0);
	area0[0] = 219; area0[1] = top;
	paintRect(area0, 0xFFE0);
	uint16_t area[] = {116, top+1, 116, top+16}, color = 0xF800;
	float_t fak = Register[17]/5.f * 3;
	uint8_t red = 32;
	uint8_t green = 32;
	for(int i=0; i<fak; i++){
		paintRect(area, color);
		area[0] += 1;
		area[2] += 1;
		if(i<32){
			red--;
			green++;
			color = (red << 11) | (green << 5);
		}
	}
}
void paint_menu(uint8_t sel){
	uint16_t color = 0xE73C;
	uint16_t pos[] = {0, 70};
	for(int i=0; i<15; i++){// Menu 0 - 14
		color = 0xE73C;
		uint16_t bit = getMenuReg(i);
		if(bit) color = 0x07E0;
		if(sel == 1){
			color = 0x835F;
			if(bit) color = 0x07FF;
		}
		setFgColor(color);
		writeText10x16(pos, Menu[i], false, false);
		pos[0] += 10 * (strlen(Menu[i]) +1);
		if((i < 14) && (pos[0] + (strlen(Menu[i+1])*10) > 319)){
			pos[1] += 16;
			pos[0] = 0;
		}
	}
	pos[0] = 0; pos[1] += 16;
	for(int i=15; i<31; i++){
		// Rahmen zeichnen
		uint8_t width = 10 * (strlen(Menu[i]) + 1);
		uint16_t area0[] = {pos[0], pos[1], pos[0]+width, pos[1]+32};
		paintRect(area0, 0x0000);
		area0[3] = pos[1];
		paintRect(area0, 0xFFE0);
		area0[2] = pos[0]; area0[3] = pos[1]+32;
		paintRect(area0, 0xFFE0);
		area0[2] = pos[0]+width; area0[1] = pos[1]+32;
		paintRect(area0, 0xFFE0);
		area0[0] = pos[0]+width; area0[1] = pos[1];
		paintRect(area0, 0xFFE0);

		pos[0] += width;
		if((i < 30) && ((pos[0] + 10*strlen(Menu[i+1])) > 319)){
			pos[0] = 0;
			pos[1] += 32;
		}
	}
}
void paint_status(){
	setOrientation(HORIZONTAL);
	setFgColor(0xE73C);setBgColor(0x0000);setSeColor(0x07E0);
	Register = si4703_get_register();
	// paint Bool Statusregister
	uint16_t pos[] = {0, 0};
	for(int i=0; i<5; i++){
		bool sel = Register[16] & (1 << (7 - i));
		writeText10x16(pos, Status[i], sel, true);
		pos[0] += strlen(Status[i]) + 2;
	}
	pos[0] = 300;
	bool sel = (Register[16] & 0x01);
	writeText10x16(pos, Status[5], sel, false);
	// paint BLERA
	setFgColor(0xFFFF);
	pos[0]=0;
	uint8_t err = (Register[16] & 0x06) >> 1;
	char text[] = "           ";
	sprintf(text, "%s=%d", Status[7],err);
	pos[1]=1;
	writeText10x16(pos, text, false, true);
	uint16_t data = (Register[20] << 8) | Register[21];
	sprintf(text, "%04X", data);
	// paint BLERB - BLERD
	pos[1]=2;
	pos[0] += 2;
	writeText10x16(pos, text, false, true);
	pos[0]=8;
	uint8_t data_reg[] = {22, 24, 26};// Hilfsvariable fuer RDS-data Register
	for(int i=8; i<11; i++){
		err = (Register[18] & (0xC0 >> (2*(i-8)))) >> (6-2*(i-8));
		sprintf(text, "%s=%d", Status[i],err);
		pos[1]=1;
		writeText10x16(pos, text, false, true);
		uint16_t data = (Register[data_reg[i-8]] << 8) | Register[data_reg[i-8] + 1];
		sprintf(text, "%04X", data);
		pos[1]=2;
		pos[0] += 2;
		writeText10x16(pos, text, false, true);
		pos[0] += 6;
	}
	// paint MHz
	setFgColor(0xFFE0);
	pos[0] = 0; pos[1] = 51;// Startzeile
	float_t channel = get_mhz();
	sprintf(text, "           ");
	writeText12x16(pos, text, false, false);
	sprintf(text, "%.2fMHz", channel);
	writeText12x16(pos, text, false, false);
	// paint rssi
	paint_rssi();
	sprintf(text, "           ");
	pos[0] = 222; pos[1] = 51;// Startzeile
	writeText12x16(pos, text, false, false);
	sprintf(text, "%d", Register[17]);
	writeText12x16(pos, text, false, false);
	paint_menu(100);
}

// Die Kontrollfunktionen
void default_setup(){
	si4703_xoscen(true);
	si4703_ahizen(true);
	si4703_write_register(12);
	busy_wait_ms(500);
	si4703_dmute(true);
	si4703_gp2(0x01);
	si4703_band(0x0);
	si4703_space(0x01);
	si4703_de(true);
	si4703_rds(true);
	si4703_volume(0x08);
	si4703_seekmode(true);
	si4703_seekth(0x00);
	si4703_skcnt(0x00);
	si4703_sksnr(0x00);
	si4703_stcien(true);
	si4703_rdsien(true);
	si4703_write_register(10);
	si4703_enable(true);
	si4703_write_register(2);
	busy_wait_ms(100);
}
void seeking(bool up){
	//seek from chan0 to chanX=(BAND_end - BAND_begin)/SPACE
	si4703_seekup(up);
	si4703_seek(true);
	si4703_write_register(2);
	//wait for gpio
	while(gpio_get(15));
//	paint_status();
	si4703_seek(false);
	si4703_write_register(2);
	paint_status();
}

int main() {
	stdio_init_all();
	busy_wait_ms(2);
	si4703_init(1, 2, 3, 14, 15);
	print_register();
	default_setup();
	print_register();
	ili9341_init();
	paint_status();
	printf("OK\n");
	while (true) {
		if(gpio_get(13)) seeking(true);
		busy_wait_ms(1);
	}
}

