#include "note.h"

bool note_is_black(int note)
{
    int pc = note % 12;
    return pc == 1 || pc == 3 || pc == 6 ||
           pc == 8 || pc == 10;
}

bool note_is_white(int note)
{
    int pc = note % 12;
    return pc == 0 || pc == 2 || pc == 4 || pc == 5 || pc == 7 || pc == 9 || pc == 11;
}

int note_white_index(int midi_note)
{
    int index = 0;
    for (int i = MIDI_NOTE_MIN; i < midi_note; i++)
    {
        if (note_is_white(i))
        {
            index++;
        }
    }
    return index;
}

int note_black_index(int midi_note)
{
    int index = 0;
    for (int i = MIDI_NOTE_MIN; i < midi_note; i++)
    {
        if (note_is_black(i))
        {
            index++;
        }
    }
    return index;
}