#pragma once
#include <stdio.h>
#include "minimp3.h"
#include <portaudio.h>

typedef struct {
	short pcm[1152*2];
	mp3dec_frame_info_t info;
}mp3Data;

void play_mp3_file(char* filename);
