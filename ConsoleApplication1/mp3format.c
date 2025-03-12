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
	play(filename);
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
		avformat_close_input(&format_context);
		return -1;
	}

	int streamIndex = -1;
	//a file can have multiple streams(for MPEG mp3 usually will be only one, 
	// so we will take the first one which is of type audio)
	for (int i = 0; i < format_context->nb_streams; i++) {
		if (format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
			streamIndex = i;
			break;
		}
	}

	if (streamIndex == -1) {
		fprintf(stderr, "None of the available streams are of type AUDIO\n");
		avformat_close_input(&format_context);
		return -1;
	}
	// AVCodecParameters describes properties about the encoded stream
	AVCodecParameters* codec_param = format_context->streams[streamIndex]->codecpar;

	int codec_id = codec_param->codec_id;
	//find the corresponding decoder for our stream
	AVCodec* found_codec = avcodec_find_decoder(codec_id);

	if (found_codec == NULL) {
		fprintf(stderr, "Codec not found");
		avformat_close_input(&format_context);
		return -1;
	}
	
	AVCodecContext* codec_context = avcodec_alloc_context3(found_codec);

	ret = avcodec_parameters_to_context(codec_context, codec_param);

	if (ret < 0) {
		fprintf(stderr, "Error on transforming codec params to codec context");
		avcodec_free_context(codec_context);
		avformat_close_input(&format_context);
		return -1;
	}

	if (avcodec_open2(codec_context, found_codec, NULL) < 0) {
		fprintf(stderr, "Error initiating the error contex");
		avcodec_free_context(codec_context);
		avformat_close_input(&format_context);
		return -1;
	}
	//structure that stores compressed data.
	AVPacket* packet = av_packet_alloc();
	//structure that describes decoded (raw) audio data.
	AVFrame* frame = av_frame_alloc();

	FILE* file = fopen("decoded.raw", "w");
	if (file == NULL) {
		fprintf(stderr, "Error on opening file to write raw data");
		avcodec_free_context(codec_context);
		avformat_close_input(&format_context);
		return -1;
	}

	//read frame by frame
	while (av_read_frame(format_context, packet) == 0) {
		//send packet to decoder,  will copy relevant AVCodecContext fields,
		if (packet->stream_index == streamIndex) {
			if (avcodec_send_packet(codec_context, packet) != 0) {
				fprintf(stderr, "Error on sending data for the decoder");
			}
			int channels = codec_context->ch_layout.nb_channels;
			int bytes_per_sample = av_get_bits_per_sample(codec_context->sample_fmt);
			while ((ret = avcodec_receive_frame(codec_context, frame)) >= 0) {
				if (av_sample_fmt_is_planar(codec_context->sample_fmt)) {
					// Handle planar audio format
					int channels = codec_context->ch_layout.nb_channels;
					int bytes_per_sample = av_get_bytes_per_sample(codec_context->sample_fmt);
					for (int ch = 0; ch < channels; ch++) {
						for (int i = 0; i < frame->nb_samples; i++) {
							fwrite(frame->data[ch] + i * bytes_per_sample, bytes_per_sample, 1, file);
						}
					}
				}
				else {
					// Handle packed audio format
					fwrite(frame->data[0], 1, frame->linesize[0], file);
				}
			}
			if (ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
				fprintf(stderr, "Error decoding audio frame\n");
				break;
			}
		}
		av_packet_unref(packet);
	}

	fclose(file);
	av_packet_free(&packet);
	av_frame_free(&frame);
	/*avcodec_*/
	//avcodec_close(codec_context);

	avcodec_free_context(&codec_context);

	avformat_close_input(&format_context);

}