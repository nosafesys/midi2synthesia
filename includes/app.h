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
    SDL_Event sdl_evt;
    smf_t *song;
    smf_event_t **smf_evts;
    int evt_c;
    int dev_c;
    int note_c;
    DeviceInfo *devs;
    PortMidiStream *stream;
    Note *notes;
    bool keys_on[NOTE_COUNT];
} App;

bool app_init(App *a);
bool app_free(App *a);
void app_device_input(App *a);
void app_file_input(App *a, char *file);

#endif