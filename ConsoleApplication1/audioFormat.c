#define _CRTDBG_MAP_ALLOC
#include "waveFormat.h"
#include <crtdbg.h>
#include <stdio.h>
#include "ui.h"
#include <taglib/tag_c.h>
#include "mp3format.h"
int main() {
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    char filename1[] = "feel_good.wav"; 
    char filename2[] = "Alesis-Fusion-Clean-Guitar-C3.wav";
    char filename3[] = "Yamaha-TG100-Ocarina-C5.wav";
    char filename[] = "InchisoareaAlba.mp3";

    //// Open the file
    //TagLib_File* file = taglib_file_new(filename);
    //if (!file) {
    //    printf("Failed to open file.\n");
    //    return 1;
    //}

    //// Retrieve metadata
    //TagLib_Tag* tag = taglib_file_tag(file);

    //// Get various metadata fields
    //const char* title = taglib_tag_title(tag);
    //const char* artist = taglib_tag_artist(tag);
    //const char* album = taglib_tag_album(tag);
    //unsigned int year = taglib_tag_year(tag);  // Use unsigned int
    //const char* genre = taglib_tag_genre(tag);
    //unsigned int track = taglib_tag_track(tag);  // Use unsigned int
    //const char* comment = taglib_tag_comment(tag);

    //// Print the metadata
    //printf("Title: %s\n", title);
    //printf("Artist: %s\n", artist);
    //printf("Album: %s\n", album);
    //printf("Year: %u\n", year);
    //printf("Genre: %s\n", genre);
    //printf("Track Number: %u\n", track);
    //printf("Comment: %s\n", comment);

    // Clean up
   // taglib_file_free(file);
    ui_start();

   play_mp3_file(filename1);
    //get_file_name();
    //playWaveFileByName(filename3);
    //ui_start();

    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
    _CrtDumpMemoryLeaks();

    return 0;
}