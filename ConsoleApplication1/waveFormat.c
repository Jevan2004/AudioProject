#include "waveFormat.h"

//reads and checks if a file is of type RIFF, filename is the name of the file
//error codes:
// -1 read data errors
// -2 file is not of RIFF / WAVE type
// 0 on no errors
int readRiffFile(FILE* file) {

	CK ck;

	//file = fopen(filename, "rb");


	//read the id of the file is an unsined long, after converting in chars each byte of the ckId it should be "RIFF" denoting that is RIFF type file
	if (fread(&ck.ckID, 1, sizeof(CKID), file) != 4) {
		perror("Error on reading chunk id");
		fclose(file);
		return -1;
	}
	// check if it's RIFF
	char letter1 = (char)(ck.ckID & 0xFF);
	char letter2 = (char)(ck.ckID >> 8 & 0xFF);
	char letter3 = (char)(ck.ckID >> 16 & 0xFF);
	char letter4 = (char)(ck.ckID >> 24 & 0xFF);

	if (letter1 != 'R' || letter2 != 'I' || letter3 != 'F' || letter4 != 'F') {
		perror("File is not of type RIFF");
		fclose(file);
		return -2;
	}

	printf("Chunk ID: %4.4s\n", (char*)&ck.ckID);
	
	// ckSize is the size of the file in bytes(-8 bytes) it does not include the fields ckSize and ckId
	if (fread(&ck.ckSize, 1, sizeof(CKSIZE), file) != 4) {
		perror("Error on reading chunk size");
		fclose(file);
		return -1;
	}

	printf("Chunk size: %lu\n", ck.ckSize);

	// read the data of the file, it should specify the type of file, it should be "WAVE" in our case
	if (fread(ck.ckData, sizeof(BYTE), 4, file) != 4) {
		perror("Error on reading chunk data");
		fclose(file);
		return -1;
	}
	ck.ckData[4] = '\0';
	printf("Chunk data : %s\n", ck.ckData);

	if (strcmp(ck.ckData, "WAVE") != 0) {
		perror("File is not of WAVE type");
		fclose(file);
		return -2;
	}

	//fclose(file);
	return 0;
}


//First we check that is a RIFF file first and a WAVE file 
//read the WAVE header info, it has 2 subchunks "fmt" and "data"
//fmt  subchunk has(number of bytes in paranthesis): 
//-Subchunk1ID(4) should conatain the letters "fmt "
//-Subchunk1Size(4) 16 for PCM
//-AudioFormat(2) PCM = 1
//-NumChannels(2) Mono = 1, Stereo = 2, etc.
//-SampleRate(4)  e.g.8000, 44100, etc.
//-ByteRate(4) == SampleRate * NumChannels * BitsPerSample / 8
//BlockAlign(2) == NumChannels * BitsPerSample / 8
//BitsPerSample(2) 8 bits = 8, 16 bits = 16, etc.
// if it's PCM it does not have extra parameters
// Subchunk 2 / the data subchunk
//-Subchunk2ID(4)      Contains the letters "data"
//-Subchunk2Size(4) == NumSamples * NumChannels * BitsPerSample / 8, this is the number of bytes in the data.
//-Data(*)   The actual sound data.
//Return NULL if there have been errors reading the data, otherwise a pointer to WAVE structure
WAVE* readWaveFile(char* filename) {
	FILE* file;

	file = fopen(filename, "rb");

	if (file == NULL) {
		perror("Error on opening file");
		return ;
	}
	//check if it is a riff file
	int result = readRiffFile(file);
	if (result == -1) {
		perror("Error on reading riff file header");
		return ;
	}

	WAVE* wave = (WAVE*)malloc(sizeof(WAVE));;
	if (wave == NULL)
	{
		perror("Eror on alocating memory");
		return;
	}
	
	DWORD subchunk1ID;
	DWORD subchunk1Size;
	WORD audioFormat;
	WORD numChannels;
	DWORD sampleRate;
	DWORD byteRate;
	WORD blockAlign;
	WORD bitsPerSample;

	//read subchunk1 id and it should be equal to "fmt "
	if (fread(&subchunk1ID, 1, sizeof(DWORD), file) != 4) {
		perror("Error on reading SubChunk1Id");
		fclose(file);
		return ;
	}

	//the letters are stored in a double word, transform them into chars
	char letter1 = (char)(subchunk1ID & 0xFF);
	char letter2 = (char)(subchunk1ID >> 8 & 0xFF);
	char letter3 = (char)(subchunk1ID >> 16 & 0xFF);
	char letter4 = (char)(subchunk1ID >> 24 & 0xFF);
	//do the checking
	if (letter1 != 'f' || letter2 != 'm' || letter3 != 't' || letter4 != ' ') {
		perror("\"fmt \" checking failed");
		fclose(file);
		return NULL;
	}

	printf("Subchunk1 id is : %4.4s\n", (char*)&subchunk1ID);
	//read the size of subchunk1
	if (fread(&subchunk1Size, sizeof(DWORD), 1, file) != 1) {
		perror("Error on reading size of Subchunk1");
		fclose(file);
		return NULL;
	}
	
	printf("Subchunk1 size is: %lu\n", subchunk1Size);
	//read the audio format
	if (fread(&audioFormat, sizeof(WORD), 1, file) != 1) {
		perror("Error on reading audio format");
		fclose(file);
		return NULL;
	}

	printf("Audio format size is: %hu\n", audioFormat);
	//read the number of channels
	if (fread(&numChannels, sizeof(uint16_t), 1, file) != 1) {
		perror("Error on reading number of channels");
		fclose(file);
		return NULL;
	}

	wave->numberOfChannels = numChannels;
	printf("Number of channels is: %hu\n", wave->numberOfChannels);

	//read the sample rate
	if (fread(&sampleRate, sizeof(DWORD), 1, file) != 1) {
		perror("Error on reading sample rate");
		fclose(file);
		return NULL;
	}
	wave->sampleRate = sampleRate;
	printf("Sample Rate is: %lu\n", wave->sampleRate);

	//read the byte rate
	if (fread(&byteRate, sizeof(DWORD), 1, file) != 1) {
		perror("Error on reading byte rate");
		fclose(file);
		return NULL;
	}

	printf("Byte Rate is: %lu\n", byteRate);
	//read the block align
	if (fread(&blockAlign, sizeof(WORD), 1, file) != 1) {
		perror("Error on reading byte rate");
		fclose(file);
		return NULL;
	}

	printf("Block align is: %hu\n", blockAlign);
	//read the bits per sample
	if (fread(&bitsPerSample, sizeof(WORD), 1, file) != 1) {
		perror("Error on reading byte rate");
		fclose(file);
		return NULL;
	}
	wave->bitsPerSample = bitsPerSample;
	printf("Bits per sample is: %hu\n", wave->bitsPerSample);

	//read the subchunk2

	//read subchunk2 id
	DWORD subchunk2Id;
	DWORD subchunk2Size;

	if (fread(&subchunk2Id, sizeof(DWORD), 1, file) != 1) {
		perror("Error on reading subchunk2 id");
		fclose(file);
		return NULL;
	}
	letter1 = (char)(subchunk2Id & 0xFF);
	letter2 = (char)(subchunk2Id >> 8 & 0xFF);
	letter3 = (char)(subchunk2Id >> 16 & 0xFF);
	letter4 = (char)(subchunk2Id >> 24 & 0xFF);
	//do the checking
	if (letter1 != 'd' || letter2 != 'a' || letter3 != 't' || letter4 != 'a') {
		perror("Subchunk2id is not equal to \"data\"");
		fclose(file);
		return NULL;
	}
	printf("Subchunk2 id is : %4.4s\n", (char*)&subchunk2Id);

	if (fread(&subchunk2Size, sizeof(DWORD), 1, file) != 1) {
		perror("Error on reading size of Subchunk2");
		fclose(file);
		return NULL;
	}
	wave->dataSize = subchunk2Size;
	printf("Subchunk2 size is: %lu\n", wave->dataSize);

	wave->data = malloc(wave->dataSize);
	if (wave->data == NULL) {
		perror("error on allocating memory!");
		fclose(file);
		return NULL;
	}
	fread(wave->data, 1, wave->dataSize, file);
	//every operation is successfull,close file, return 0
	fclose(file);
	return wave;
}
//the callback function which is called by PortAudio whenever it captured data/needs more data for output
static int audioCallBack(const void* inputBuffer, void* outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void* userData) {
	//that's our wave data
	WAVE* wave = (WAVE*)userData;
	//we don't receive data from microphone
	(void)inputBuffer;
	//that will show us how much of the data we played
	static int audioPosition = 0;
	//more chanells -> more data
	uint32_t bytesPerFrame = (wave->bitsPerSample / 8) * wave->numberOfChannels;

	uint32_t bytesToCopy = bytesPerFrame * framesPerBuffer;

	if (audioPosition + bytesToCopy > wave->dataSize) {
		bytesToCopy = wave->dataSize - audioPosition;
	}
	//copy the data to be play at this moment
	memcpy(outputBuffer, wave->data + audioPosition, bytesToCopy);

	audioPosition += bytesToCopy;
	//return paComplete if the audio is finished playing
	if (audioPosition >= wave->dataSize) {
		return paComplete;
	}
	else
	{
		return paContinue;
	}
}
//play the wave file
void playWaveFile(WAVE* wave) {
	//check the format(if the audio data is stored on 8 or 16 bits
	int format = 0;
	if (wave->bitsPerSample == 8) {
		format = paUInt8;
	}
	else {
		if (wave->bitsPerSample == 16) {
			format = paInt16;
		}
	}
	//initialize port audio
	Pa_Initialize();
	//declare a stream
	PaStream* stream;
	//open stream
	Pa_OpenDefaultStream(&stream, 0, wave->numberOfChannels, format, wave->sampleRate, paFramesPerBufferUnspecified, audioCallBack, wave);
	//start the stream
	Pa_StartStream(stream);
	//while the stream is active, sleep so it can have time playing to sound

	//stop and close the stream
	Pa_StopStream(stream);
	Pa_CloseStream(stream);
	//terminate port audio
	Pa_Terminate();
}

void playWaveFileByName(char* filename) {

	WAVE* wave = readWaveFile(filename);

	playWaveFile(wave);

	free(wave->data);
	free(wave);
}