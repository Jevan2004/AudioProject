#include "mp3format.h"
#define MINIMP3_IMPLEMENTATION
#include "minimp3.h"

static int audioCallBack1(const void* inputBuffer, void* outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void* userData) {

	mp3Data* data = (mp3Data*)userData;

	short* out = (short*)outputBuffer;
	(void)inputBuffer;

	static int audioPosition = 0;

	uint32_t how_much_to_copy = framesPerBuffer * data->info->channels;

	for (uint32_t i = 0; i < how_much_to_copy; i++) {
		if (audioPosition < data->data_size) {
			out[i] = data->pcm_data[audioPosition];
			//printf("%hi", data->pcm_data[audioPosition]);
			audioPosition+= 1;
		}

	}
	if (audioPosition < data->data_size) {
		return paContinue;
	}
	else
		return paComplete;
}


void play_mp3_file(char* filename) {

	//open mp3 file
	FILE* file = fopen(filename, "rb");

	if (file == NULL) {
		perror("Error on opening file");
		return;
	}
	//determine file size in bytes
	fseek(file, 0, SEEK_END);
	long size;
	size = ftell(file);
	//move cursor at beggining of the file
	rewind(file);
	//buffer for the mp3 file
	unsigned char* input_buffer = (unsigned char*)malloc(size);

	if (!input_buffer) {
		perror("Error on allocating memory");
		fclose(file);
		return;
	}
	//read the mp3 file into the buffer
	fread(input_buffer, 1, size, file);
	fclose(file);
	//initialize the mp3 decoder
	static mp3dec_t mp3d;
	mp3dec_init(&mp3d);

	short* all_pcm_data = (short*)malloc(1024 * sizeof(short));
	if (all_pcm_data == NULL) {
		perror("Error on allocating memory");
		fclose(file);
		return;
	}
	int pcm_data_size = 0;
	int buffer_size = 1024;

	mp3dec_frame_info_t info;
	short pcm[MINIMP3_MAX_SAMPLES_PER_FRAME];
	int samples = 0;
	// how much we proccessed of the file
	int offset = 0;
	//decode mp3 frame by frame
	while (offset < size) {
		samples = mp3dec_decode_frame(&mp3d, input_buffer + offset, size - offset, pcm, &info);
		printf("Channels %d, hertz %d, bitrate %d\n", info.channels, info.hz, samples);
		for (int i = 0; i < samples; i++) {
			if (pcm_data_size >= buffer_size) {
				buffer_size *= 2;
				short* reallocation;
				reallocation = (short*)realloc(all_pcm_data, buffer_size * sizeof(short));
				if (all_pcm_data == NULL) {
					perror("eror on reallocating memory");
					fclose(file);
					return;
				}
				all_pcm_data = reallocation;
			}
			all_pcm_data[pcm_data_size] = pcm[i];
			pcm_data_size++;
		}


		offset += info.frame_bytes;
	}

	mp3Data* mp3 = (mp3Data*)malloc(sizeof(mp3Data));

	if (mp3 == NULL) {
		perror("error on allocating memory");
		return;
	}

	mp3->pcm_data = all_pcm_data;
	mp3->data_size = pcm_data_size;
	mp3->info = &info;

	Pa_Initialize();
	PaStream* stream;

	Pa_OpenDefaultStream(&stream, 0, info.channels, paInt16, info.hz, paFramesPerBufferUnspecified, audioCallBack1, mp3);

	Pa_StartStream(stream);

	while (Pa_IsStreamActive(stream)) {
		Pa_Sleep(1000);
	}

	Pa_StopStream(stream);
	Pa_CloseStream(stream);
	Pa_Terminate();

	free(all_pcm_data);
	free(mp3);
}