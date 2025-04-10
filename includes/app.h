#ifndef APP_H
#define APP_H

#include "main.h"
#include "midi.h"
#include "note.h"

typedef struct App
{
    bool running;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;
    smf_t *smf;
    int device_count;
    DeviceInfo *devices;
    PortMidiStream *stream;
    Note notes[MAX_NOTES];
    bool key_active[NOTE_COUNT];
    int note_count;
} App;

bool app_init(App *a);
bool app_free(App *a);
bool app_device_input(App *a);
bool app_file_input(App *a);

#endif