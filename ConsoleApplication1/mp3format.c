#include "mp3format.h"
#define MINIMP3_IMPLEMENTATION
#include "minimp3.h"


static int audioCallBack(const void* inputBuffer, void* outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void* userData) {

	mp3Data* data = (mp3Data*)userData;


	(void)inputBuffer;

	static int audioPosition = 0;

	

}

void play_mp3_file(char* filename) {

	FILE* file = fopen(filename, "rb");

	if (file == NULL) {
		perror("Error on opening file");
		return;
	}

	fseek(file, 0, SEEK_END);
	long size;
	size = ftell(file);
	rewind(file);

	unsigned char* input_buffer = (unsigned char*)malloc(size);

	if (!input_buffer) {
		perror("Error on allocating memory");
		fclose(file);
		return;
	}

	fread(input_buffer, 1, size, file);
	fclose(file);

	static mp3dec_t mp3d;
	mp3dec_init(&mp3d);

	mp3dec_frame_info_t info;
	short pcm[MINIMP3_MAX_SAMPLES_PER_FRAME];
	int samples = 0;

	int offset = 0;

	while (offset < size) {
		samples = mp3dec_decode_frame(&mp3d, input_buffer + offset, size - offset, pcm, &info);
		printf("Channels %d, hertz %d, bitrate %d\n", info.channels, info.hz, samples);


		offset += info.frame_bytes;
	}

}
