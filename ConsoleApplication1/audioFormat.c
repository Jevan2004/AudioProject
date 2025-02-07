#include "waveFormat.h"
#include <crtdbg.h>
#include <stdio.h>
#include "ui.h"
int main() {

    char filename[] = "feel_good.wav"; 
    char filename2[] = "Alesis-Fusion-Clean-Guitar-C3.wav";
    char filename3[] = "Yamaha-TG100-Ocarina-C5.wav";


    //get_file_name();
    //playWaveFileByName(filename3);
    ui_start();
    //_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtDumpMemoryLeaks();

    return 0;
}