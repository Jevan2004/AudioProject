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
// checks if a file is of format mp3
// returns:
// 0 on success
// <0 on failure
// -1 error on finding the format
// -2 file is not of mp3 format
int check_if_mp3(const char* filename) {
	//AVFormatContext is a structure which holds information about the file, handles input/output formats and streams,
	//also hold info about metadata
	AVFormatContext* format_contex = NULL;
	//opens a stream for the input file, ( usully will get also the header data,not for the audio data, but because mp3 is MPEG
	//it does not have a header, but frames which need to be decoded
	int ret = avformat_open_input(&format_contex, filename, NULL, NULL);
	//with av_strerror we get the error from the error codes of the ffmpeg functions
	if (ret < 0) {
		char errorbf[130];
		av_strerror(ret, errorbf, sizeof(errorbf));
		fprintf(stderr, "%s", errorbf);
		return -1;
	}
	//read packets of media to get some metadata
	ret = avformat_find_stream_info(format_contex, NULL);
	if (ret < 0) {
		char errorbf[130];
		av_strerror(ret, errorbf, sizeof(errorbf));
		fprintf(stderr, "%s", errorbf);
		avformat_close_input(&format_contex);
		return -1;
	}
	//check if the file format is of type mp3
	if (strcmp(format_contex->iformat->name, "mp3") != 0) {
		fprintf(stderr,"The input file is not of type mp3");
		avformat_close_input(&format_contex);
		return -2;
	}
	
	avformat_close_input(&format_contex);
	return 0;
}

void play_mp3_file(char* filename) {

	check_if_mp3(filename);
}

int play(const char* filename) {
	AVFormatContext* format_context = NULL;

	int ret = avformat_open_input(&format_context, filename, NULL, NULL);

	if (ret < 0) {
		char errorbuf[130];
		av_strerror(ret, errorbuf, sizeof(errorbuf));
		fprintf(stderr, "%s", errorbuf);
		return -1;
	}

	ret = avformat_find_stream_info(format_context, NULL);

	if (ret < 0) {
		char errorbuf[130];
		av_strerror(ret, errorbuf, sizeof(errorbuf));
		fprintf(stderr, "%s", errorbuf);
		return -1;
	}

	int streamIndex = -1;
	//a file can have multiple streams(for MPEG mp3 usually will be only one, 
	// so we will take the first one which is of type audio)
	for (int i = 0; i < format_context->nb_streams; i++) {
		if (format_context->streams[i]->codecpar->codec_type != AVMEDIA_TYPE_AUDIO) {
			streamIndex = i;
			break;
		}
	}

	if (streamIndex == -1) {
		fprintf(stderr, "None of the available streams are of type AUDIO\n");
		avformat_close_input(&format_context);
		return -1;
	}

}