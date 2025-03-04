#include "ui.h"
#include "waveFormat.h"

void display_title() {
	printf("---------Wave file player---------\n");
	printf("Welcome to my app!\n");
}

char* get_file_name() {

	char* filename = (char*)malloc(sizeof(char) * 128);
	if (filename == NULL) {
		perror("Error on alocating memory");
		return NULL;
	}
	while (1) {
		printf("Enter the wav file you want to play(must contain .wav extension):\n");
		fgets(filename, 128, stdin);
		filename[strlen(filename) - 1] = '\0';
		if (!strstr(filename, ".wav")) {
			printf("The file does not have .wav extension\n");
		}
		if(check_if_file_exists(filename) == 0){
			break;
		}
	}
	return filename;
}

int check_if_file_exists(char* filename) {
	FILE* file = fopen(filename, "rb");

	if (file == NULL) {
		printf("This file does not exist\n");
		return -1;
	}

	fclose(file);
	return 0;
}

void display_options() {
	printf("Choose an option:\n");
	printf("1.Play a wav file\n");
	printf("2.Exit\n");
}

void playWav(char* filename) {

	playWaveFileByName(filename);

	free(filename);

	printf("The wav file has been played\n");
}

void ui_start() {

	display_title();
	char option[20];
	char* filename = NULL;
	while (1) {
		display_options();
		fgets(option, 20, stdin);
		option[strlen(option) - 1] = '\0';
		if (strcmp(option, "1") == 0) {
			filename = get_file_name();
			playWav(filename);
		}
		else
			if (strcmp(option, "2") == 0) {

				break;
			}
			else {
				printf("Wrong option\n");
			}
	}
}