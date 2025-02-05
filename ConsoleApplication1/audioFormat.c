
#include "waveFormat.h"
#include <stdio.h>

int main() {
    //PaError err = Pa_Initialize();
    //if (err != paNoError) {
    //    std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
    //    return 1;
    //}

    //std::cout << "PortAudio initialized successfully!" << std::endl;

    //Pa_Terminate();
    // 
    char filename[] = "feel_good.wav"; // Allocate 15 bytes (including '\0')
    //if (!filename) {
    //    perror("Memory allocation failed");
    //    return 1;
    //}
    //strcpy(filename, "feel_good.wav"); // Safe copy


    readWaveFile(filename);

    //free(filename);

    return 0;
}