/*
 * Note processing implementation
 * Handles operations related to musical notes, such as rendering and state management
 */

#include "note.h"

/*
 * Determine if a MIDI note corresponds to a black key
 * Returns true if the note is black, false otherwise
 */
bool note_is_black(int midi_note)
{
    // Black keys follow a specific pattern on the keyboard
    int mod = midi_note % 12;
    return mod == 1 || mod == 3 || mod == 6 || mod == 8 || mod == 10;
}

/*
 * Determine if a MIDI note corresponds to a white key
 * Returns true if the note is white, false otherwise
 */
bool note_is_white(int midi_note)
{
    // White keys are the complement of black keys
    return !note_is_black(midi_note);
}

/*
 * Get the index of a black key on the keyboard
 * Returns the index of the black key
 */
int note_black_index(int midi_note)
{
    int idx = 0;
    // Count the number of black keys up to the given MIDI note
    for (int i = MIDI_NOTE_MIN; i < midi_note; i++)
    {
        if (note_is_black(i))
        {
            idx++;
        }
    }
    return idx;
}

/*
 * Get the index of a white key on the keyboard
 * Returns the index of the white key
 */
int note_white_index(int midi_note)
{
    int idx = 0;
    // Count the number of white keys up to the given MIDI note
    for (int i = MIDI_NOTE_MIN; i < midi_note; i++)
    {
        if (note_is_white(i))
        {
            idx++;
        }
    }
    return idx;
}