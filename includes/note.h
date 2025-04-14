/*
 * Note processing definitions and declarations
 * Includes structures and function prototypes for musical notes
 */

#ifndef NOTE_H
#define NOTE_H

#include "main.h"

typedef struct
{
    int md_note;
    int idx;
    bool on;
    bool black;
    bool marked;
    double start_t;
    double end_t;
    int y;
    int h;
    int vel;

} Note;

bool note_is_black(int midi_note);
bool note_is_white(int midi_note);
int note_black_index(int midi_note);
int note_white_index(int midi_note);

#endif