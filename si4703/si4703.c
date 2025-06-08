// si4703 Bibliothek
#include "si4703.h"

uint8_t BUS, SDA, SCL, RST, GP2, ADDR = 0x10;
uint8_t REGISTER[32]; // Starts by PowerCnfReg. 02h upper-byte

void read_register(){
	uint8_t buf[32];
	int ret=i2c_read_blocking(I2C_INSTANCE(BUS), ADDR, buf, 32, false);
	for(int i=16; i<32; i++) REGISTER[i-16] = buf[i];
	for(int i=0; i<16; i++) REGISTER[i+16] = buf[i];
}
// for Debug
void print_register(){
	read_register();
	for(int i=0; i<32; i=i+2){
		printf("Reg 0x%02X: 0x%02X%02X\n", i/2+2, REGISTER[i], REGISTER[i+1]);
	}
}

uint8_t *si4703_get_register(){
	read_register();
	return REGISTER;
}

void si4703_write_register(uint8_t cnt){
	i2c_write_blocking(I2C_INSTANCE(BUS), ADDR, REGISTER, cnt, false);
}

void si4703_enable(bool on){
	if(on) REGISTER[1] = 0x01;
	else REGISTER[1] = 0x41;
}

void si4703_dsmute(bool disable){
	if(disable) REGISTER[0] |= 0x80;
	else REGISTER[0] &= 0x7F;
}

void si4703_dmute(bool disable){
	if(disable) REGISTER[0] |= 0x40;
	else REGISTER[0] &= 0xBF;
}

void si4703_mono(bool mono){
	uint mask = 0x20;
	if(mono) REGISTER[0] |= 0x20;
	else REGISTER[0] &= 0xDF;
}

void si4703_rdsm(bool verbose){
	uint mask = 0x08;
	if(verbose) REGISTER[0] |= 0x08;
	else REGISTER[0] &= 0xF7;
}

void si4703_seekmode(bool stop){
	uint mask = 0x04;
	if(stop) REGISTER[0] |= 0x04;
	else REGISTER[0] &= 0xFB;
}

void si4703_seekup(bool up){
	if(up) REGISTER[0] |= 0x02;
	else REGISTER[0] &= 0xFD;
}

void si4703_seek(bool seek){
	if(seek) REGISTER[0] |= 0x01;
	else REGISTER[0] &= 0xFE;
}

void si4703_tune(bool tune){
	if(tune) REGISTER[2] |= 0x80;
	else REGISTER[2] &= 0x7F;
}

void si4703_chan(uint16_t chan){
	REGISTER[3] |= chan;
	REGISTER[2] |= chan >> 8;
}

void si4703_rdsien(bool enable){
	if(enable) REGISTER[4] |= 0x80;
	else REGISTER[4] &= 0x7F;
}

void si4703_stcien(bool set){
	if(set) REGISTER[4] |= 0x40;
	else REGISTER[4] &= 0xBF;
}

void si4703_rds(bool on){
	if(on) REGISTER[4] |= 0x10;
	else REGISTER[4] &= 0xEF;
}

void si4703_de(bool de){
	if(de) REGISTER[4] |= 0x08;
	else REGISTER[4] &= 0xF7;
}

void si4703_agcd(bool disable){
	if(disable) REGISTER[4] |= 0x04;
	else REGISTER[4] &= 0xFB;
}

void si4703_blndadj(uint8_t adj){
	REGISTER[5] &= 0x3F;
	REGISTER[5] |= adj << 6;
}

void si4703_gp3(uint8_t set){
	REGISTER[5] &= 0xCF;
	REGISTER[5] |= set << 4;
}

void si4703_gp2(uint8_t set){
	REGISTER[5] &= 0xF3;
	REGISTER[5] |= set << 2;
}

void si4703_gp1(uint8_t set){
	REGISTER[5] &= 0xFC;
	REGISTER[5] |= set;
}

void si4703_seekth(uint8_t th){
	REGISTER[6] = th;
}

void si4703_band(uint8_t band){
	REGISTER[7] &= 0x3F;
	REGISTER[7] |= band << 6;
}

void si4703_space(uint8_t space){
	REGISTER[7] &= 0xCF;
	REGISTER[7] |= space << 4;
}

void si4703_volume(uint8_t vol){
	REGISTER[7] &= 0xF0;
	REGISTER[7] |= vol;
}

void si4703_smuter(uint8_t rate){
	REGISTER[8] &= 0x3F;
	REGISTER[8] |=  rate << 6;
}

void si4703_smutea(uint8_t att){
	REGISTER[8] &= 0xCF;
	REGISTER[8] |=  att << 4;
}

void si4703_rdsprf(bool enable){
	if(enable) REGISTER[8] |= 0x02;
	else REGISTER[4] &= 0xFD;
}

void si4703_volext(bool enable){
	if(enable) REGISTER[8] |= 0x01;
	else REGISTER[4] &= 0xFE;
}

void si4703_sksnr(uint8_t th){
	REGISTER[9] &= 0x0F;
	REGISTER[9] |= th << 4;
}

void si4703_skcnt(uint8_t cnt){
	REGISTER[9] &= 0xF0;
	REGISTER[9] |= cnt;
}

void si4703_xoscen(bool enable){
	if(enable) REGISTER[10] |= 0x80;
	else REGISTER[10] &= 0x7F;
}

void si4703_ahizen(bool enable){
	if(enable) REGISTER[10] |= 0x40;
	else REGISTER[10] &= 0xBF;
}

void si4703_init(uint8_t bus, uint8_t sda, uint8_t scl, uint8_t rst, uint8_t gp2){
	BUS = bus; SDA = sda; SCL = scl; RST = rst;GP2 = gp2;
	if(1){// init the ports for Startcondition
		gpio_init(rst);
		gpio_init(gp2);
		gpio_init(sda);
		gpio_init(scl);
		gpio_set_pulls(gp2, 0, 1);
		gpio_set_pulls(rst, 0, 1);
		gpio_set_pulls(sda, 0, 1);
		gpio_set_pulls(scl, 0, 1);
		gpio_set_dir(rst, 1);
		gpio_set_dir(sda, 1);
		gpio_set_dir(scl, 1);
		gpio_put(sda, 0);
		gpio_put(scl, 0);
		gpio_put(rst, 0);
		busy_wait_ms(1);
		gpio_put(rst, 1);
		busy_wait_ms(1);
		gpio_deinit(sda);
		gpio_deinit(scl);
		i2c_init(I2C_INSTANCE(bus), 100*1000);
		gpio_set_function(sda, GPIO_FUNC_I2C);
		gpio_set_function(scl, GPIO_FUNC_I2C);
		gpio_pull_up(sda);
		gpio_pull_up(scl);
		gpio_pull_up(rst);
		read_register();// Wichtig, s kann das Board nicht funktionieren
	}
}

