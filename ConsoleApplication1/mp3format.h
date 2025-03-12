#pragma once
#include <stdio.h>
#include "minimp3.h"
#include <portaudio.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>

typedef struct {
	short* pcm_data;
	int data_size;
	mp3dec_frame_info_t* info;
}mp3Data;

void play_mp3_file(char* filename);
