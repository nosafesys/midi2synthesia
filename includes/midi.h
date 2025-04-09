#ifndef MIDI_H
#define MIDI_H

#include "main.h"
typedef struct App App;

typedef struct
{
    char *name;
    int input_support;
} DeviceInfo;

bool midi_device_count(App *a);
bool midi_device_info(App *a, int device_count);
bool midi_open_stream(App *a, int device_id);
void midi_poll_events(App *a);

#endif