#pragma once
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <portaudio.h>

//structure for the RIFF header
typedef unsigned long DWORD;
typedef unsigned char BYTE;
typedef unsigned short int WORD;

typedef DWORD FOURCC; // Four-character code
typedef FOURCC CKID; // Four-character-code chunk identifier
typedef DWORD CKSIZE; // 32-bit unsigned size value
typedef struct { // Chunk structure
	CKID ckID; // Chunk type identifier
	CKSIZE ckSize; // Chunk size field (size of ckData)
	BYTE ckData[5]; // Chunk data
} CK;

typedef struct {
	uint16_t numberOfChannels;
	uint32_t sampleRate;
	uint16_t bitsPerSample;
	uint32_t dataSize;
	uint8_t* data;
}WAVE;

WAVE* readWaveFile(char* filename);
void playWaveFileByName(char* filename);