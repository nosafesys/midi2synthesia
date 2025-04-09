#ifndef NOTE_H
#define NOTE_H

#include "main.h"

typedef struct
{
    int midi_note;
    int index;
    bool active;
    bool black;
    int y;
    int height;
    int velocity;
} Note;

bool note_is_black(int midi_note);
bool note_is_white(int midi_note);
int note_black_index(int midi_note);
int note_white_index(int midi_note);

#endif