/* Zuvo Graphik in GIMP erstellen
 * dann in GIMP: Bild->Modus=RGB, Bild->Genaugkeit=8bit, Alphakanal entfernen,
 * export mit Dateiendung ".data" --> Speicher=Standart Palette=Normal
 *
 * Dieses Programm wandelt die 24Bit-RGB-Pixel einer Eingabedatei
 * in R5G6B5-Pixel um die viele LCD-Displays darstellen koennen,
 * und schreibt sie als text in eine Headerdatei.
 * Die Headerdatei enthält ein Datenarray aus uint8 Werten welche die
 * Bitfolge Hexadezimal darstellt. So können diese Daten mit der
 * "#include"-Anweisung in Binarys mit einkompiliert werden.
 * Wenn die daten in einen ili9314-LCD angezeigt ewrden. muessen nur die
 * ersten fuenf code-Zeilen editiert werden. Ansonsten ist in
 * Code-Zeile 53 das erste Byte (hier 0x2C) entsprechend zu aendern.
 * Als Beispielt dient hier fraktal_240x135.data, das Programm muss mit
 * "gcc -o rgb2header rgb2header.c" kompiliert werden damit es
 * ausfuehrbar wird */

#include <stdio.h>
#include <stdlib.h>

/* Eine Graphik konvertieren */
int einfach(){
	// Breite und hoehe des Bildes in Pixel
	int width = 7;
	int height = 10;

	/* Wert = 30 wenn width und und height beide zweistellig sind.
	 * Mit jeder Stelle muss der text[Wert] angepasst werden
	 * eine Stelle weniger Wert=29, eine mehr Wert=31 */
	char text[32];

	// Dateiname der Eingabedatei
	FILE *fileIn = fopen("d_0.data", "rb");

	// Dateiname der Ausgabe-Headerdatei
	FILE *fileOut = fopen("d_0.h", "w");


	if(fileOut==0){printf("Ausgabe-Dateifehler\n");return 0;}
	if(fileIn==0){printf("Eingabe-Dateifehler\n");return 0;}

	// Die Eingabedatei (R8G8B8-Pixelfolge) wird in data gespeichert
	int data_len = width*height*3;// Drei Byte/Pixel
	unsigned char data[data_len];
	fread(data, 1, data_len, fileIn);

	/* DATA_LEN kann im Programm Verwendet werden um
	 * nicht selbst rechen zu muessen*/
	sprintf(text, "#define BILD_LEN (%d*%d*2+1)\n", width, height);
	fprintf(fileOut, text);
	fprintf(fileOut, "static uint8_t BILD[BILD_LEN] ={\n");

	/* Das ili_9341 erwartet als Startbyte der darzustellenden
	 * Daten den Wert 0x2C */
	fprintf(fileOut, "0X2C,\n");// Startbyte ili9341

	//read 3 bytes and write 1 word (16bit)
	// Es muessen 3 Byte/Pixel parallel verarbeitet werden
	for(int m=0; m<data_len-2; m=m+3){

		//Damit die Headerdatei fuer den Menschen bessr lesbar ist
		if(m%4==0&&m>0)fprintf(fileOut, " ");
		if(m%24==0&&m>0)fprintf(fileOut, "\n");

		/* word = 0x0000 (RRRR RGGG GGGB BBBB)
		 * Die Farbbytes werden um ihre oberen Bits gekuerzt und
		 * an die entsprechende Stelle innerhalb der 2 Bytes geschoben*/
		u_int16_t blue = (data[m+2] >>3) & 0x1F;
		u_int16_t green = ((data[m+1] >>2) & 0x3F)<<5;
		u_int16_t red = ((data[m] >>3) & 0x1F)<<11;
		u_int16_t sum = red | green | blue;
		fprintf(fileOut, "0x%02X,0x%02X,", (sum&0xFF00)>>8, (sum&0x00FF));
	}
	fclose(fileIn);
	fprintf(fileOut, "};\n");
	fclose(fileOut);
}

/* Beispiel einer Kolonnenvrarbeitung mehrer Graphiken in einen Header
 * ueber den dann programmatisch iteriert werden kann.
 * Die Eingabe-Dateinamen lauten fortlaufend "d0_15x20.data, d1_15x20.data, .."
 * die Anzahl der Bilder ist 10 im Format 10x15 und am Ende wird ein
 * schwarzes Bild eingefuegt. Der Header enthaelt ein zwei-
 * dimensionales Arrea das elf uint8-Arreas enthaelt
 * Die Funktion muss unten in der main-Funktion aktiviert werden */
int kolonne(){
	int data_len = 7*10*3;
	FILE *fileH = fopen("digit7x10.h", "w");
	if(fileH==0){printf("Dateifehler h\n");return 0;}
	fprintf(fileH, "#define DIGIT_LEN (7*10*2+1)\n");
	fprintf(fileH, "static uint8_t DIGIT[11][DIGIT_LEN] ={\n");

	//5r6g5b pixelformat (3Ah =03h)
	for(int i=0; i<10; i++){
		char filename[14];
		sprintf(filename, "d_%d.data", i);
		FILE *fileData = fopen(filename, "rb");
		if(fileData==0){printf("Dateifehler d\n");return 0;}
		unsigned char data[data_len];
		fread(data, 1, data_len, fileData);
		fprintf(fileH, "{0X2C, // %d\n", i);// Startbyte ili9341
		//read 3 bytes and write 1 word
		for(int m=0; m<data_len-2; m=m+3){
			if(m%4==0&&m>0)fprintf(fileH, " ");
			if(m%24==0&&m>0)fprintf(fileH, "\n");
			//word 0x0000;//RRRR RGGG GGGB BBBB
			u_int16_t blue = (data[m+2] >>3) & 0x1F;
			u_int16_t green = ((data[m+1] >>2) & 0x3F)<<5;
			u_int16_t red = ((data[m] >>3) & 0x1F)<<11;
			u_int16_t sum = red | green | blue;
			fprintf(fileH, "0x%02X,0x%02X,", (sum&0xFF00)>>8, (sum&0x00FF));
		}
		fprintf(fileH, "},\n");
		fclose(fileData);
	}
	fprintf(fileH, "{0X2C, // BLACK/EMPTY\n");
	for(int m=0; m<data_len-2; m=m+3){
		if(m%4==0&&m>0)fprintf(fileH, " ");
		if(m%24==0&&m>0)fprintf(fileH, "\n");
		//word 0x0000;//RRRR RGGG GGGB BBBB
		fprintf(fileH, "0x%02X,0x%02X,", 0, 0);
	}
	fprintf(fileH, "},\n");
	fprintf(fileH, "};\n");
	fclose(fileH);
}

int main(){
//	einfach();
	kolonne();
}
