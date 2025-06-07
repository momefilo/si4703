#include <stdio.h>
#include <stdlib.h>
void make_txt(){
	FILE *fileOut = fopen("charset.txt", "w");
	char zeichen[1];
	for(int i=32; i<128; i++){
		sprintf(zeichen, "%c", i);
		fprintf(fileOut, zeichen);
		printf("%s", zeichen);
	}
	fclose(fileOut);
}
int main(){
	int char_width = 14;
	int char_height = 20;
	int char_count = 94;
	int char_len = char_width * char_height * 3;

	// Dateiname der Ausgabe-Headerdatei
	FILE *fileOut = fopen("font14x20.h", "w");
	fprintf(fileOut, "static uint8_t FONT14x20[94][32] ={\n");

	// Dateiname der Eingabedatei
	FILE *fileIn = fopen("font14x20.data", "rb");
	// Die Eingabedatei (R8G8B8-Pixelfolge) wird in data gespeichert
	if(fileOut==0){printf("Ausgabe-Dateifehler\n");return 0;}
	if(fileIn==0){printf("Eingabe-Dateifehler\n");return 0;}

	int data_len = char_count * char_width * char_height * 3;// Drei Byte/Pixel
	unsigned char data[data_len];
	fread(data, 1, data_len, fileIn);
	for(int c=0; c<char_count; c++){
		fprintf(fileOut, "{\n");
		for(int row=0; row<char_height; row++){
			char print_byte1 = 0, print_byte2 = 0;;
			char print_text[15] = {"              "};
			for(int col=0; col<char_width; col++){
				int d=c*char_width*3+row*char_width*94*3+col*3;
				if(data[d]>0 || data[d+1]>0 || data[d+2]>0){
					if(col < 6){
						print_byte1 |= 1 << (5 - col);
						print_text[col] = '#';
					}
					else{
						print_byte2 |= 1 << (13 - col);
						print_text[col] = '#';
					}
				}
			}
			fprintf(fileOut, "0x%02X,0x%02X, // %s\n", print_byte1, print_byte2, print_text);
		}
		fprintf(fileOut, "},\n");
	}
	fprintf(fileOut, "};\n");
	fclose(fileOut);
	fclose(fileIn);
	return 0;
}
