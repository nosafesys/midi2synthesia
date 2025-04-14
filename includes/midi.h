/*
 * MIDI processing definitions and declarations
 * Includes structures and function prototypes for MIDI handling
 */

#ifndef MIDI_H
#define MIDI_H

#include "main.h"
typedef struct App App;

typedef struct
{
    char *name;
    int inp;
} DeviceInfo;

bool midi_device_count(App *a);
bool midi_device_info(App *a, int device_count);
bool midi_open_stream(App *a, int device_id);
bool midi_load_file(App *a, char *filename);
void midi_poll_events(App *a);
void midi_get_notes(App *a);

#endif