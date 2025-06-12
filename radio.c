// radio
#include "buttons/buttons.h"
#include "ili_9341/ili_9341.h"
#include "rds.h"
//#include "tmc_events.h"
#include "si4703/si4703.h"
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
//#include "pico/stdio.h"
#include "pico/double.h"

#define GP_IRQ 15

uint8_t *Register;// Das Register des si4703
uint8_t Selektion = 33;// Der aktuell selektierte Menueintrag

//vorwärtsdeklarationen
void clearRDSData();
void displayRDSText(uint8_t tpy);
uint16_t displayFreq();

bool showRDS = true;
bool IfSel = true;// wird das RDS-Menu angezeigt?
// Die Menu-Texte die angezeigt werden
char Status[28][6] = {
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
	"PN",//		16 4bit
	"ID",//		17 12bit
	"REV",//	18 6bit
	"DEV",//	19 4bit
	"FW",//		20 6bit
	"REG2",//	21 7bit
	"REG3",//	22 5bit
	"REG4",//	23 3bit
	"REG6",//	24 3bit
	"REG7",//	25 12bit
	"REG8",//	26 16bit
	"REG9",//	27 16bit
};
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
	".BAND.",//15 2bit
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
	"ON",// 27 bool
	"VOL",// 28 4bit
	"SEEKUP",// 29 bool
	"SEEK",// 	30 bool
};
char *HumanText[8][4] = {
	{"87-108", "76-108", "76-90", "res"},//15 BAND
	{"200kHz", "100kHz", "50kHz", "res"},//	16 SPACE
	{"31-49", "37-55", "19-37", "25-43"},//	17 BLNDADJ
	{"blitz", "fast", "norm", "slow"},//	21 SMUTER
	{"16 db", "14 db", "12 db", "10 db"},//	22 SMUTEA
	{"HiI", "M/S", "Low", "Hig"},//			23 GP3
	{"HiI", "IrE", "Low", "Hig"},//			24 GP2
	{"HiI", "res", "Low", "Hig"}//			25 GP1
};
char **getHumanText(uint8_t nr){
	if(nr==15) return HumanText[0];
	else if(nr==16) return HumanText[1];
	else if(nr==17) return HumanText[2];
	else if(nr==21) return HumanText[3];
	else if(nr==22) return HumanText[4];
	else if(nr==23) return HumanText[5];
	else if(nr==24) return HumanText[6];
	else if(nr==25) return HumanText[7];
}

// Helferfunktionen fuer die Menu-Texte
uint16_t getStatusReg(uint8_t nr){// Liest aus Register
	switch(nr){
		case 0: return Register[16] & 0x80; break;// RDSR
		case 1: return Register[16] & 0x40; break;// STC
		case 2: return Register[16] & 0x20; break;// SF/BL
		case 3: return Register[16] & 0x10; break;// AFCRL
		case 4: return Register[16] & 0x08; break;//RDSS
		case 5: return Register[16] & 0x01; break;//ST
		case 6: return Register[17]; break;//RSSI
		case 7: return Register[16] & 0x06; break;//BLERA
		case 8: return (Register[18] & 0xC0)>>4; break;//BLERB
		case 9: return (Register[18] & 0x30)>>4; break;//BLERC
		case 10: return Register[18] & 0x0C; break;//BLERD
		case 11: return (Register[19] | ((Register[18] & 0x03)<<8)); break;//READCHAN
		case 12: return (Register[21] | ((Register[20])<<8)); break;//RDSA
		case 13: return (Register[23] | ((Register[22])<<8)); break;//RDSB
		case 14: return (Register[25] | ((Register[24])<<8)); break;//RDSC
		case 15: return (Register[27] | ((Register[26])<<8)); break;//RDSD
		case 16: return ((Register[28] & 0xF0)>>4); break;// PN
		case 17: return (((Register[28] & 0x0F)<<8) | Register[29]); break;// MFGID
		case 18: return ((Register[30] & 0xFC)>>2); break;// REV
		case 19: return (((Register[30] & 0x03)<<2) | ((Register[31] & 0xC0)>>6)); break;// DEV
		case 20: return ((Register[31] & 0x3F)); break;// FIRMWARE
		case 21: return (((Register[0] & 0x10)) | ((Register[1] & 0x20)>>2) | ((Register[1] & 0x0E)>>1)); break;// REG2
		case 22: return ((Register[2] & 0x7C)>>2); break;// REG3
		case 23: return (((Register[4] & 0x20)>>3) | (Register[4] & 0x03)); break;// REG4
		case 24: return ((Register[8] & 0x0E)>>1); break;// REG6
		case 25: return ((Register[10] & 0x3F)<<8) | (Register[11]); break;// REG7
		case 26: return (Register[12] <<8) | (Register[13]); break;// REG8
		case 27: return (Register[14] <<8) | (Register[15]); break;// REG9
	}
}
uint16_t getMenuReg(uint8_t nr){// Liest aus Register
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
		case 15: return (Register[7] & 0xC0)>>6; break;//BAND
		case 16: return (Register[7] & 0x30)>>4; break;//SPACE
		case 17: return (Register[5] & 0xC0)>>6; break;//BLNDADJ
		case 18: return Register[6]; break;//SEEKTH
		case 19: return (Register[9] & 0xF0)>>4; break;//SKSNR
		case 20: return Register[9] & 0x0F; break;//SKCNT
		case 21: return (Register[8] & 0xC0)>>6; break;//SMUTER
		case 22: return (Register[8] & 0x30)>>4; break;//SMUTEA
		case 23: return (Register[5] & 0x30)>>4; break;//GP3
		case 24: return (Register[5] & 0x0C)>>2; break;//GP2
		case 25: return Register[5] & 0x03; break;//GP1
		case 26: return Register[3]; break;//CHAN
		case 27: return Register[1] & 0x01; break;//ENABLE
		case 28: return Register[7] & 0x0F; break;//VOLUME
		case 29: return Register[0] & 0x02; break;//SEEKUP
		case 30: return Register[0] & 0x01; break;//SEEK
	}
}
float_t get_mhz(){// Liest aus Register
	uint16_t chan = getStatusReg(11);
	float_t space = 0.2;
	if(Register[7] & 0x10) space = 0.1;
	if(Register[7] & 0x20) space = 0.05;
	float_t ofset = 76.0;
	if( (Register[7] & 0xC0) == 0) ofset = 87.5;
	return chan * space + ofset;
}
void setMenuReg(uint8_t nr, uint16_t data){// Schreibt in das Register von si4703.h
	if(nr == 0) si4703_dsmute(data);
	else if(nr == 1) si4703_dmute(data);
	else if(nr == 2) si4703_mono(data);
	else if(nr == 3) si4703_rdsm(data);
	else if(nr == 4) si4703_seekmode(data);
	else if(nr == 5) si4703_rdsien(data);
	else if(nr == 6) si4703_stcien(data);
	else if(nr == 7) si4703_rds(data);
	else if(nr == 8) si4703_de(data);
	else if(nr == 9) si4703_agcd(data);
	else if(nr == 10) si4703_tune(data);
	else if(nr == 11) si4703_ahizen(data);
	else if(nr == 12) si4703_volext(data);
	else if(nr == 13) si4703_rdsprf(data);
	else if(nr == 14) si4703_xoscen(data);
	else if(nr == 15) si4703_band(data);
	else if(nr == 16) si4703_space(data);
	else if(nr == 17) si4703_blndadj(data);
	else if(nr == 18) si4703_seekth(data);
	else if(nr == 19) si4703_sksnr(data);
	else if(nr == 20) si4703_skcnt(data);
	else if(nr == 21) si4703_smuter(data);
	else if(nr == 22) si4703_smutea(data);
	else if(nr == 23) si4703_gp3(data);
	else if(nr == 24) si4703_gp2(data);
	else if(nr == 25) si4703_gp1(data);
	else if(nr == 28) si4703_volume(data);
}
char *getRegText(uint8_t nr){
	return getHumanText(nr)[getMenuReg(nr)];
}

// Malfunktionen
void paint_rssi(){
	uint8_t top = 74;// Startzeile
	uint16_t left = 125;
	uint16_t area0[] = {left, top, left+114, top+16};
	paintRect(area0, 0x0000);
	area0[3] = top;
	paintRect(area0, 0xFFE0);
	area0[2] = left; area0[3] = top+16;
	paintRect(area0, 0xFFE0);
	area0[2] = left+114; area0[1] = top+16;
	paintRect(area0, 0xFFE0);
	area0[0] = left+114; area0[1] = top;
	paintRect(area0, 0xFFE0);
	uint16_t area[] = {left+1, top+1, left+1, top+16}, color = 0xF800;
	float_t fak = Register[17]/5.f * 3;
	uint8_t red = 32;
	uint8_t green = 0;
	for(int i=0; i<fak; i++){
		paintRect(area, color);
		area[0] += 1;
		area[2] += 1;
		if(i<32){
			green +=2;
			color = (red << 10) | (green << 5);
		}
		if(i>15 && i<47){
			red--;
			color = (red << 10) | (green << 5);
		}
	}
}
void paintRahmen(uint16_t left, uint16_t top, uint16_t width, uint16_t height, uint16_t color){
	uint16_t area[] = {left, top, left+width, top+height};
	paintRect(area, color);//leeren
	uint16_t area2[] = {left+1, top+1, left+width-1, top+height-1};
	paintRect(area2, 0x0000);//leeren
}
void paintBoolMenu(bool org, bool sel){
	paintRahmen(140, 0, 40, 40, 0x835F);
	char text[4];
	uint16_t pos[] = {145, 4};
	if(org && sel){ setFgColor(0x07FF); setSeColor(0xE73C);}
	else if(org && !sel){ setFgColor(0x07E0); setSeColor(0x835F);}
	else if(!org && sel){ setFgColor(0x835F); setSeColor(0x07E0);}
	else if(!org && !sel){ setFgColor(0xE73C); setSeColor(0x07FF);}
	sprintf(text, "ON");
	writeText10x16(pos, text, false, false);
	pos[1] = 20;
	sprintf(text, "OFF");
	writeText10x16(pos, text, true, false);
}
void paintOptMenu(uint8_t org, uint8_t sel){
	uint8_t left = 120; uint8_t top = 0; uint8_t width = 80; uint8_t height = 80;
	paintRahmen(left, top, width, height, 0x835F);
	//paint options
	uint16_t color_onon = 0x07FF, color_onoff = 0x07E0, color_offon = 0x835F, color_offoff = 0xE73C;
	char **text = getHumanText(Selektion);
	uint16_t pos[] = {left + 5, top + 4};
	for(int i=0; i<4; i++){
		if(i==org){
			if(i==sel) setFgColor(color_onon); else setFgColor(color_onoff);
		}
		else if(i==sel) setFgColor(color_offon);
		else setFgColor(color_offoff);
		writeText10x16(pos, text[i], false, false);
		pos[1] += 16;
	}
}
void paintAnaMenu(uint8_t wert, bool u8){
	uint8_t left = 140; uint8_t top = 0; uint8_t width = 40; uint8_t height = 40;
	if(u8){ left = 120; width = 80;}
	char text[7];
	sprintf(text, "0x%01X", wert);
	if(u8) sprintf(text, "0x%02X", wert);
	paintRahmen(left, top, width, height, 0x835F);
	uint16_t pos[] = {left + 5, top + 10};
	setFgColor(0xFFFF);
	writeText10x16(pos, text, false, false);

}

// Das Userinterface
void paint_menu(uint8_t sel){// Zeichnet das Eingabemenu unten im Display
	// paint tune up and down
	uint16_t pos[] = {0, 98};
	char text[10];
	setFgColor(0xFFE0); setSeColor(0x835F);
	sprintf(text, "DOWN");
	bool nr = false;
	if(sel == 29) nr = true;
	writeText12x16(pos, text, nr, false);
	pos[0] = 60;
	sprintf(text, "UP");
	if(sel == 30) nr = true; else nr = false;
	writeText12x16(pos, text, nr, false);
	pos[0] = 96;
	sprintf(text, "PREV");
	if(sel == 31) nr = true; else nr = false;
	writeText12x16(pos, text, nr, false);
	pos[0] = 156;
	sprintf(text, "NEXT");
	if(sel == 32) nr = true; else nr = false;
	writeText12x16(pos, text, nr, false);
	pos[0] = 220;
	sprintf(text, "RDS");
	if(sel == 33) nr = true; else nr = false;
	writeText12x16(pos, text, nr, false);
	uint16_t color = 0xE73C;
	pos[1] = 125; pos[0] = 0;
	// Menu 0 - 14
	for(int i=0; i<15; i++){
		color = 0xE73C;
		uint16_t bit = getMenuReg(i);
		if(bit) color = 0x07E0;
		if(sel == i){
			color = 0x835F;
			if(bit) color = 0x07FF;
		}
		setFgColor(color);
		writeText7x11(pos, Menu[i], false, false);
		pos[0] += 7 * (strlen(Menu[i]) +1);
		if((i < 14) && (pos[0] + (strlen(Menu[i+1])*7) > 319)){
			pos[1] += 12;
			pos[0] = 0;
		}
	}
	// Menu 15 - 28
	pos[0] = 0; pos[1] += 16;
	for(int i=15; i<29; i++){
		// Rahmen zeichnen
		color = 0xE73C;
		uint8_t width = 8 * (strlen(Menu[i]) + 1);
		uint16_t area0[] = {pos[0], pos[1], pos[0]+width, pos[1]+32};
		paintRect(area0, 0x0000);
		area0[3] = pos[1];
		paintRect(area0, color);
		area0[2] = pos[0]; area0[3] = pos[1]+32;
		paintRect(area0, color);
		area0[2] = pos[0]+width; area0[1] = pos[1]+32;
		paintRect(area0, color);
		area0[0] = pos[0]+width; area0[1] = pos[1];
		paintRect(area0, color);

		// Text zeichnen oben
		setFgColor(0xFFFF);
		if(sel == i) setFgColor(0x835F);
		uint16_t pos_2[] = {pos[0]+5, pos[1]+2};
		writeText7x11(pos_2, Menu[i], false, false);
		// Text zeichnen unten
		pos_2[1] += 16;
		if(i==18 || i==19 || i==20 || i==26 || i==27 || i==28)
			sprintf(text, "%d", getMenuReg(i));
		else sprintf(text, "%s", getRegText(i));
		writeText7x11(pos_2, text, false, false);

		pos[0] += width;
		if((i < 30) && ((pos[0] + 8 * strlen(Menu[i+1])) > 319)){
			pos[0] = 0;
			pos[1] += 32;
		}
	}
}
void paint_status(){// Zeichnet die Statuszeilen oben im Display
	setOrientation(HORIZONTAL);
	setFgColor(0xFFFF);setBgColor(0x0000);setSeColor(0x07E0);
	Register = si4703_get_register();
	// paint Register 0 - 1 and secret Bits of the other Registers
	uint16_t pos[] = {0, 0};
	char text[] = "           ";
	for(int i=16; i<28; i++){
		sprintf(text, "%s=0x%02X",Status[i], getStatusReg(i));
		if(i==17 || i>24)sprintf(text, "%s=0x%04X",Status[i], getStatusReg(i));
		writeText7x11(pos, text, false, false);
		pos[0] += (strlen(text)+1)*7;
		if((i < 27) && (pos[0] + 7 * ((strlen(Status[i+1]))+4) > 319)){
			pos[0] = 0;
			pos[1] += 11;
		}
	}
	// paint BLERA - BLERD
	pos[1] += 13;
	pos[0] = 0;
	for(int i=7; i<11; i++){
		sprintf(text, "%s=%d", Status[i],getStatusReg(i));
		writeText7x11(pos, text, false, false);
		sprintf(text, "%04X", getStatusReg(i+5));
		pos[1] += 11; pos[0] += 14;
		writeText7x11(pos, text, false, false);
		pos[0] += 75; pos[1] -= 11;
	}
	// paint Bool Statusregister
	setFgColor(0xF79C);
	pos[1] += 24; pos[0] = 0;
	for(int i=0; i<6; i++){
		bool sel = getStatusReg(i);
		writeText7x11(pos, Status[i], sel, false);
		pos[0] += ((strlen(Status[i])+2)*9);
		if((pos[0] + 7 * ((strlen(Status[i+1]))) > 319)){
			pos[0] = 0;
			pos[1] += 11;
		}
	}
	// paint MHz
	setFgColor(0xFFE0);
	pos[0] = 0; pos[1] = 74;// Startzeile
	float_t channel = get_mhz();
	sprintf(text, "           ");
	writeText12x16(pos, text, false, false);
	sprintf(text, "%.2fMHz", channel);
	writeText12x16(pos, text, false, false);
	// paint rssi
	paint_rssi();
	sprintf(text, "           ");
	pos[0] = 245; pos[1] = 74;// Startzeile
	writeText12x16(pos, text, false, false);
	sprintf(text, "%d", Register[17]);
	writeText12x16(pos, text, false, false);
	paint_menu(Selektion);
}

// Die Kontrollfunktionen fuer das Radio
void default_setup(){
	si4703_xoscen(true);
	si4703_ahizen(true);
	si4703_write_register(12);
	busy_wait_ms(500);
	si4703_dmute(true);
	si4703_gp2(0x01);
	si4703_band(0x0);
	si4703_space(0x02);
	si4703_de(true);
	si4703_rds(true);
	si4703_volume(0x08);
	si4703_seekmode(true);
	si4703_seekth(0x19);
	si4703_skcnt(0x04);
	si4703_sksnr(0x08);
	si4703_stcien(true);
	si4703_rdsien(true);
	si4703_write_register(10);
	si4703_enable(true);
	si4703_write_register(2);
	busy_wait_ms(100);
}
void seeking(bool up){
	si4703_seekup(up);
	si4703_seek(true);
	si4703_write_register(2);
	//wait for gpio ready seek
	while(gpio_get(GP_IRQ));
	busy_wait_ms(60);
	si4703_get_register();
	si4703_seek(false);
	si4703_write_register(2);
	while(si4703_get_register()[16] & 0x40){busy_wait_ms(250);}
	clearRDSData();
	//paint_status();
}
void tuning(bool up){// TODO not working
	// Read the channel and  plus/minus
	uint16_t chan = getStatusReg(11);
	printf("0x%04X %b\n", chan, up);
	float_t space = 0.2;
	if(Register[7] & 0x10) space = 0.1;
	if(Register[7] & 0x20) space = 0.05;
	float_t ofset = 76.0;
	if( (Register[7] & 0xC0) == 0) ofset = 87.5;
	if(up){
		if(chan < ((108 - ofset) / space)) chan++;
		else chan = 0;
	}else{
		if(chan > 0) chan--;
		else chan = (108 - ofset) / space;
	}
	// Set the Register
	printf("0x%04X\n", chan);
	si4703_chan(chan);
	si4703_tune(true);
	si4703_write_register(4);
	//wait for gpio ready tune
	while(gpio_get(GP_IRQ));
	busy_wait_ms(60);
	paint_status();
	si4703_tune(false);
	si4703_write_register(4);
	while(getStatusReg(1)){ busy_wait_ms(500);paint_status();}
	clearRDSData();
	printf("TUNE COMPLETE\n");
}

// Die Kontrollfunktionen fuer das Menu
void changeSel(uint8_t button){
	if(button==BUTTON_U){ if(Selektion > 6) Selektion -= 7; else Selektion = 33; }
	else if(button==BUTTON_D){ Selektion += 7; if(Selektion > 33) Selektion = 0; }
	else if(button==BUTTON_L){ if(Selektion > 0)Selektion--; else Selektion = 33; }
	else if(button==BUTTON_R){ Selektion++; if(Selektion > 33) Selektion = 0; }
	paint_menu(Selektion);
}
void takeSel(uint8_t nr){
	if(nr == 13){
		if(Selektion == 29) seeking(false);
		else if(Selektion == 30) seeking(true);
		else if(Selektion == 31) tuning(false);
		else if(Selektion == 32) tuning(true);
		else if(Selektion == 33){// RDS
			showRDS = true;
			IfSel = true;
			clearScreen();
			busy_wait_ms(100);
			displayFreq();
		}
		else if(Selektion < 15){// Die 15 Bool-Werte des menus
			bool sel = getMenuReg(Selektion);
			bool togle = sel;
			paintBoolMenu(sel, togle);
			uint8_t button = get_Button();
			while(button < BUTTON_R || button > BUTTON_M){
				button = get_Button();
				if(button == BUTTON_U || button == BUTTON_D){ togle = !togle; paintBoolMenu(sel, togle);}
			}
			clearScreen();
			setMenuReg(Selektion, togle);
			if(button == BUTTON_R) si4703_write_register(12);// Das Register wird geschrieben
			busy_wait_ms(6);
			paint_status();
		}
		else if(Selektion < 18 || (Selektion > 20 && Selektion < 26)){// Die acht 4-Optionen-Werte
			uint16_t org = getMenuReg(Selektion);
			uint16_t togle = org;
			paintOptMenu(org, togle);
			uint8_t button = get_Button();
			while(button < BUTTON_R || button > BUTTON_M){
				button = get_Button();
				if(button == BUTTON_U){ if(togle > 0) togle--; else togle = 3; paintOptMenu(org, togle);}
				else if(button == BUTTON_D){ if(togle < 3) togle++; else togle = 0; paintOptMenu(org, togle);}
			}
			clearScreen();
			setMenuReg(Selektion, togle);
			if(button == BUTTON_R) si4703_write_register(12);// Das Register wird geschrieben
			busy_wait_ms(6);
			paint_status();
		}
		else if(Selektion < 21 || Selektion == 28){// Die vier Analogwerte
			bool u8 = false; uint8_t max = 15;
			if(Selektion == 18){ u8 = true; max = 0x7F;}
			uint16_t wert = getMenuReg(Selektion);
			paintAnaMenu(wert, u8);
			uint8_t button = get_Button();
			while(button < BUTTON_R || button > BUTTON_M){
				button = get_Button();
				if(button == BUTTON_U){ if(wert < max) wert++; else wert = 0; paintAnaMenu(wert, u8);}
				else if(button == BUTTON_D){ if(wert > 0) wert--; else wert = max; paintAnaMenu(wert, u8);}
			}
			clearScreen();
			setMenuReg(Selektion, wert);
			if(button == BUTTON_R) si4703_write_register(12);// Das Register wird geschrieben
			busy_wait_ms(6);
			paint_status();

		}
	}
}

// Das RDS-Menu
uint8_t RDSText[129];
uint16_t RDSText_flag = 0;
uint8_t RDSText_ABflag = 0;
uint8_t StationName[11];
uint8_t StationName_flag = 0;
void clearRDSData(){// wird von seeking und tuning aufgeerufen
	for(int i=0; i<129; i++){
		if(i < 11) StationName[i] = 0;
		RDSText[i]=0;
	}
}
uint16_t displayFreq(){
	char text[15];
	bool sel = false;
	if(getStatusReg(5)) sel = true;// Stereo
	sprintf(text, "%.2f", get_mhz());
	uint16_t len = (strlen(text)) * 14 + 5;
	uint16_t pos[] = {4, 4};
	setFgColor(0xFFFF); setSeColor(0x07E0);
	writeText14x20(pos, text, sel, false);
	return len;
}
void displayPS(uint8_t flags){
	if(showRDS){
		setFgColor(0xFFFF); setSeColor(0x07E0);
		char text[15];
		sprintf(text, "%.2f", get_mhz());
		uint16_t len = (strlen(text)) * 14 + 5 + 4;
		uint16_t pos[] = {len, 4};
		bool sel = false;
		sprintf(text, "%s", StationName);
		writeText14x20(pos, text, false, false);
		busy_wait_ms(1);

		pos[0] += (strlen(text)) * 14 + 5;
		pos[1] = 2;
		sprintf(text, "%s", "TP");
		sel = (flags & 0x40);
		writeText7x11(pos, text, sel, false);

		sprintf(text, "%s", "TA");
		pos[1] = 14;
		sel = (flags & 0x20);
		writeText7x11(pos, text, sel, false);

		sprintf(text, "%s", "Musik");
		if(flags & 0x10) sprintf(text, "%s", "Speek");
		pos[0] += 17;
		pos[1] = 2;
		writeText7x11(pos, text, true, false);

		sprintf(text, "%s", "dPTY");
		pos[1] = 14;
		sel = (flags & 0x08);
		writeText7x11(pos, text, sel, false);

		pos[0] += 5 * 7 + 2;
		pos[1] = 2;
		sprintf(text, "%s", "comp");
		sel = (flags & 0x04);
		writeText7x11(pos, text, sel, false);

		sprintf(text, "%s", "arti");
		pos[1] = 14;
		sel = (flags & 0x02);
		writeText7x11(pos, text, sel, false);

		pos[0] += 4 * 7 + 2;
		pos[1] = 2;
		sprintf(text, "%s", "ST");
		sel = (flags & 0x01);
		if(sel) sprintf(text, "%s", "MO");
		writeText7x11(pos, text, !sel, false);

	}
}
void displayRDSText(uint8_t pty){
	if(showRDS){
		uint16_t pos[] = {4, 25};
		setFgColor(0xF7DF);
		writeText12x16(pos, PTY[pty], false, false);
		setFgColor(0xFFFF);
		paintRahmen(0, 44, 319, 2, 0x835F);
		paintRahmen(0, 85, 319, 2, 0x835F);
		int zeileNr = 50;
		for(int i=0; i<4; i++){
			uint8_t zeile[32];
			int k = 0;
			for(int j=i*32; j<i*32+32; j++){
				zeile[k] = RDSText[j];
				k++;
			}
			uint16_t pos[2] = {0, zeileNr};
			writeText10x16(pos, zeile, false, false);
			zeileNr += 17;
		}
	}
}

// Die RDS-Decodierung. Siehe GNURadio
void RDS(uint gpio, uint32_t events){
	uint8_t *reg = si4703_get_register();
	if((reg[16] & 0x80)){// RDSR bit is set
		uint8_t group = ((reg[22] & 0xF0) >> 4);
		char version = 'A';
		if(reg[22] & 0x08) version = 'B';
		if(group == 0){// PS Stationname
			bool tp = (reg[22] & 0x04);
			bool ta = (reg[23] >>  4) & 0x01;
			bool ms = (reg[23] >>  3) & 0x01;
			bool di = (reg[23] >>  2) & 0x01;
			bool dpty, comp, arti, most;
			uint8_t segment = (reg[23] & 0x03);
			char ps_1 = reg[26];
			char ps_2 = reg[27];
			if(segment == 0 && di) dpty = true;
			else if(segment == 1 && di) comp = true;
			else if(segment == 2 && di) arti = true;
			else if(segment == 3 && di) most = true;
			uint8_t psFlags = (tp << 6);
			psFlags |= (ta << 5); psFlags |= (ms << 4);
			psFlags |= (dpty << 3); psFlags |= (comp << 2); psFlags |= (arti << 1); psFlags != most;
			if(StationName_flag & (1 << segment)){// das Segment wurde bereits empfangen
				if((StationName[segment * 2] != ps_1) || (StationName[segment * 2 + 1] != ps_2)){
					StationName[segment * 2] = ps_1;
					StationName[segment * 2 + 1] = ps_2;
					StationName_flag = (1 << segment);
				}
			}
			else {
				//printf("%d, %x\n", segment, StationName_flag);
				StationName[segment * 2] = ps_1;
				StationName[segment * 2 + 1] = ps_2;
				StationName_flag |= (1 << segment);
			}
			if(StationName_flag == 0xF) displayPS(psFlags);
		}
		else if(group == 1){// PIN
			uint8_t ecc    = 0;
			uint8_t paging = 0;
			uint8_t country_code = (reg[20] >> 4) & 0x0f;
			uint8_t paging_codes = reg[23] & 0x1f;
			uint8_t variant_code = (reg[24] >> 8) & 0x7;
			uint16_t slow_labelling = (reg[24] << 8) | reg[25];
			uint8_t day    = ((reg[26] >> 3) & 0x1f);
			uint8_t hour   = ((reg[26] & 0x07) <<  2) | ((reg[27] & 0xC0) >> 6);
			uint8_t minute = (reg[27] & 0x3f);
			//printf("Group 1\n");

		}
		else if(group == 2){// rds-text
			uint8_t pty = ((reg[22] & 0x03) << 3) | ((reg[23] & 0xE0) >> 5);
			uint8_t segment = (reg[23] & 0x0f);
			uint8_t seg_width = 4;
			uint8_t seg_valid = 0;
			bool abflag = (reg[23] >>  4) & 0x01;
			if(RDSText_ABflag != abflag) RDSText_flag = 0;// neuer Inhalt
			RDSText_ABflag = abflag;
			if(version == 'A'){
				RDSText[segment * 4] = reg[24];
				RDSText[segment * 4 + 1] = reg[25];
				RDSText[segment * 4 + 2] = reg[26];
				RDSText[segment * 4 + 3] = reg[27];
			}else{
				seg_width = 2;
				RDSText[segment * 2] = reg[26];
				RDSText[segment * 2 + 1] = reg[27];
			}
			RDSText_flag |= (1 << segment);
			for(int i=0; i<16; i++){ if(RDSText_flag & (1 << i)) seg_valid++; else break; }
			char *tail = (char *)memchr(RDSText, '\r', seg_valid * seg_width);
			if(tail || seg_valid == 16) displayRDSText(pty);
		}
		/*
		else if(group == 3 && version == 'A'){// Anwendung für offene Daten ODA
			uint8_t app_group = (reg[23] >> 1) & 0xf;
			bool group_type        =  reg[23] & 0x1;
			uint16_t message          =  (reg[24] << 8) | reg[25];
			uint16_t aid              =  (reg[26] << 8) | reg[27];
			char text[33];
			if(!group_type && app_group == 8){
				uint8_t var_code = (message >> 14) & 0x3;
				if(var_code == 0){
					uint8_t ltn = (message >> 6) & 0x3f; // location table number
					bool afi 	= (message >> 5) & 0x1;  // alternative freq. indicator
					bool M   	= (message >> 4) & 0x1;  // mode of transmission
					bool I   	= (message >> 3) & 0x1;  // international
					bool N   	= (message >> 2) & 0x1;  // national
					bool R   	= (message >> 1) & 0x1;  // regional
					bool U   	=  message       & 0x1;  // urban
					sprintf(text, "T:%02X af:%b M:%b i:%b n:%b r:%b u:%b", \
						ltn, afi, M, I, N, R, U);
				}
				else if(var_code==1) {
					uint8_t G   = (message >> 12) & 0x3;  // gap
					uint8_t sid = (message >>  6) & 0x3f; // service identifier
					uint8_t gap_no[4] = {3, 5, 8, 11};
					sprintf(text, "gap:%d groups SID:0x%02x", gap_no[G], sid);
				}
			}
			else sprintf(text, "Reg C: 0x%04X Reg D: 0x%04X", message, aid);
			//printf("Group 3: %s\n", text);
		}
		else if(group == 4 && version == 'A'){// Time
			uint8_t hours   = ((reg[25] & 0x1) << 4) | ((reg[26] >> 4) & 0x0f);
			uint8_t minutes =  (((reg[27] & 0xC0) >> 6) | ((reg[26] & 0x0F) << 2)) & 0x3f;
			double_t local_time_offset = .5 * (reg[27] & 0x1f);

			if(reg[27] & 0x20) local_time_offset *= -1;
			double_t julian_date = (((reg[23] & 0x03) << 15) | (((reg[24] << 8) | reg[25]) >> 1) & 0x7fff);

			uint year  = (julian_date - 15078.2) / 365.25;
			uint month = (julian_date - 14956.1 - (year * 365.25)) / 30.6001;
			uint day   =  julian_date - 14956.1 - (year * 365.25) - (month * 30.6001);
			hours += local_time_offset;
			if(hours > 23){ hours = 0; day++;}
			bool K = ((month == 14) || (month == 15)) ? 1 : 0;
			year += K;
			month -= 1 + K * 12;
			//printf("Time %d.%d.%d %d:%d\n", (day+1), month, (year+1900), hours, minutes);
		}
		else if(group == 8 && version == 'A'){// ODA
			bool T = (reg[23] >> 4) & 0x1; // 0 = user message, 1 = tuning info
			bool F = (reg[23] >> 3) & 0x1; // 0 = multi-group, 1 = single-group
			bool D = (reg[24] >> 7) & 0x1; // 1 = diversion recommended
			char text[32];
			if(T) { // tuning info
				// lout << "#tuning info# ";
				uint8_t variant = reg[22] & 0xf;
				if((variant > 3) && (variant < 10)) {
					//lout << "variant: " << variant << " - "
					//	<< group[2] << " " << group[3] << std::endl;
					sprintf(text, "Tuning-Info %0X%0X%0X%0X", reg[24], reg[25], reg[26], reg[27]);
				} else {
					//lout << "invalid variant: " << variant << std::endl;
					sprintf(text, "Invalid Variant");
				}

			}
			else if(F || D) { // single-group or 1st of multi-group
				uint8_t dp_ci    = reg[23]        & 0x7;   // duration & persistence or continuity index
				bool sign        = (reg[24] >> 6) & 0x1;   // event direction, 0 = +, 1 = -
				uint8_t extent   = (reg[24] >> 3) & 0x7;   // number of segments affected
				uint16_t event   = ((reg[24] << 8) | reg[25]) & 0x7ff; // event code, defined in ISO 14819-2
				uint16_t location = ((reg[26] << 8) | reg[27]);                // location code, defined in ISO 14819-3
				//lout << "#user msg# ";
				if(F) {
					if (D) {
						//lout << "diversion recommended, ";
					}
					//lout << "single-grp, duration:" << tmc_duration[dp_ci][0];
					sprintf(text, "TMC %s", tmc_duration[dp_ci][0]);
				} else {
					//lout << "multi-grp, continuity index:" << dp_ci;
					sprintf(text, "dp_ci 0x%0x", dp_ci);
				}
				int event_line = tmc_event_code_index[event][1];
				//lout << ", extent:" << (sign ? "-" : "") << extent + 1 << " segments"
				//	<< ", event" << event << ":" << tmc_events[event_line][1]
				//	<< ", location:" << location << std::endl;
				sprintf(text, "%s", tmc_events[event_line][1]);

			}
			else { // 2nd or more of multi-group
				uint8_t ci = reg[23] & 0x7;          // countinuity index
				bool sg = (reg[24] >> 6) & 0x1;          // second group
				uint8_t gsi = (reg[24] >> 4) & 0x3; // group sequence
				uint free_format[] = {0, 0, 0, 0};
				uint no_groups = 0;
				//lout << "#user msg# multi-grp, continuity index:" << ci
				//	<< (sg ? ", second group" : "") << ", gsi:" << gsi;
				//lout << ", free format: " << (group[2] & 0xfff) << " "
				//	<< group[3] << std::endl;
				// it's not clear if gsi=N-2 when gs=true
				sprintf(text, "optional content");
				if(sg) {
					no_groups = gsi;
				}
				free_format[gsi] = (((reg[25] | reg[24] << 8) & 0xfff) << 12) | (reg[27] | (reg[26] << 8));
				if(gsi == 0) {
					//decode_optional_content(no_groups, free_format);
					int label          = 0;
					int content        = 0;
					int content_length = 0;
					int ff_pointer     = 0;

					for (int i = no_groups; i == 0; i--){
						ff_pointer = 12 + 16;
						while(ff_pointer > 0){
							ff_pointer -= 4;
							label = (free_format[i] >> ff_pointer) & 0xf;
							content_length = optional_content_lengths[label];
							ff_pointer -= content_length;
							content = (free_format[i] >> ff_pointer) & ((1 << content_length) - 1);
							//lout << "TMC optional content (" << label_descriptions[label]
							//	<< "):" << content << std::endl;
							//printf("OPCont: %s %x\n",label_descriptions[label], content);
						}
					}
				}
			}
			//printf("%s\n", text);
		}*/

	}
}

void volume(bool up){

}
int main() {
	stdio_init_all();
	busy_wait_ms(2);
	buttons_init();
	si4703_init(1, 2, 3, 14, 15);
	default_setup();
	Register = si4703_get_register();
	ili9341_init();
	displayFreq();
	gpio_init(GP_IRQ);
	gpio_set_irq_enabled_with_callback(GP_IRQ, GPIO_IRQ_EDGE_RISE, true, RDS);
	while (true) {
		uint8_t button = get_Button();
		if(button < 13){
			if(! IfSel) changeSel(button);
			else{
				if(button == BUTTON_L){
					seeking(false);
					paintRahmen(0, 0, 319, 86, 0x0000);
					displayFreq();
				}
				else if(button == BUTTON_R){
					seeking(true);
					paintRahmen(0, 0, 319, 87, 0x0000);
					displayFreq();
				}
				if(button == BUTTON_U){
					volume(true);
				}
				else if(button == BUTTON_D){
					volume(true);
				}
			}
		}
		else if(button < 14){
			if(! IfSel) takeSel(button);
			else{
				showRDS = false;
				IfSel = false;
				clearScreen();
				busy_wait_ms(10);
				paint_status();
			}
		}
		busy_wait_ms(1);
	}
}

