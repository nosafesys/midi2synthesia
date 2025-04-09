#ifndef APP_H
#define APP_H

#include "main.h"
#include "midi.h"
#include "note.h"

typedef struct App
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;
    bool running;
    int note_count;
    int device_count;
    DeviceInfo *devices;
    PortMidiStream *stream;
    Note notes[MAX_NOTES];
    bool key_active[NOTE_COUNT];
} App;

#endif