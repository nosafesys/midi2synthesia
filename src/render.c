/*
 * Rendering implementation
 * Handles graphical rendering of notes and keys
 */

#include "render.h"
#include "app.h"
#include "note.h"

/*
 * Render white notes on the screen
 * Iterates through the notes and draws white notes
 */
void render_white_notes(App *a)
{
    for (int i = 0; i < a->note_c; i++)
    {
        // Skip notes that are black or have zero height
        if (a->notes[i].h == 0 || a->notes[i].black)
            continue;

        // Calculate the x-coordinate for the note
        int x = a->notes[i].idx * (WHITE_KEY_WIDTH + 2) + 25;

        // Define the rectangle for the note
        SDL_Rect rect = {x, a->notes[i].y, WHITE_KEY_WIDTH - 1, a->notes[i].h};

        // Draw the note with rounded corners
        roundedBoxRGBA(a->renderer, rect.x, rect.y, rect.x + rect.w, rect.y + rect.h,
                       RADIUS, 0, 255, 100, 255);
    }
}

/*
 * Render black notes on the screen
 * Iterates through the notes and draws black notes
 */
void render_black_notes(App *a)
{
    for (int i = 0; i < a->note_c; i++)
    {
        // Skip notes that are white or have zero height
        if (a->notes[i].h == 0 || !a->notes[i].black)
            continue;

        // Calculate the x-coordinate for the note
        int shift_w_idx = note_white_index(a->notes[i].md_note) - 1;
        int x = shift_w_idx * (WHITE_KEY_WIDTH + 2) + 25 + WHITE_KEY_WIDTH - (BLACK_KEY_WIDTH / 2);

        // Define the rectangle for the note
        SDL_Rect rect = {x, a->notes[i].y, BLACK_KEY_WIDTH, a->notes[i].h};

        // Draw the note with rounded corners
        roundedBoxRGBA(a->renderer, rect.x, rect.y, rect.x + rect.w, rect.y + rect.h,
                       RADIUS, 0, 120, 40, 255);
    }
}

/*
 * Render white keys on the screen
 * Draws the white keys of the piano
 */
void render_white_keys(App *a)
{
    for (int i = MIDI_NOTE_MIN; i <= MIDI_NOTE_MAX; i++)
    {
        // Check if the note corresponds to a white key
        if (note_is_white(i))
        {
            // Calculate the index and position of the key
            int idx = note_white_index(i);
            SDL_FRect rect = {
                .x = idx * (WHITE_KEY_WIDTH + 2) + 25,
                .y = SCREEN_HEIGHT_LARGE - WHITE_KEY_HEIGHT,
                .w = WHITE_KEY_WIDTH,
                .h = WHITE_KEY_HEIGHT};

            // Set the color based on whether the key is pressed
            if (a->keys_on[i])
            {
                SDL_SetRenderDrawColor(a->renderer, 0, 255, 100, 255);
            }
            else
            {
                SDL_SetRenderDrawColor(a->renderer, 255, 255, 255, 255);
            }

            // Draw the key
            SDL_RenderFillRect(a->renderer, &rect);
        }
    }
}

/*
 * Render black keys on the screen
 * Draws the black keys of the piano
 */
void render_black_keys(App *a)
{
    for (int i = MIDI_NOTE_MIN; i <= MIDI_NOTE_MAX; i++)
    {
        // Check if the note corresponds to a black key
        if (note_is_black(i))
        {
            // Calculate the position of the key
            int shift_w_idx = note_white_index(i) - 1;
            int x = shift_w_idx * (WHITE_KEY_WIDTH + 2) + 25 + WHITE_KEY_WIDTH - (BLACK_KEY_WIDTH / 2);

            SDL_FRect rect = {
                .x = x,
                .y = SCREEN_HEIGHT_LARGE - WHITE_KEY_HEIGHT,
                .w = BLACK_KEY_WIDTH,
                .h = BLACK_KEY_HEIGHT};

            // Set the color based on whether the key is pressed
            if (a->keys_on[i])
            {
                SDL_SetRenderDrawColor(a->renderer, 0, 120, 40, 255);
            }
            else
            {
                SDL_SetRenderDrawColor(a->renderer, 0, 0, 0, 255);
            }

            // Draw the key
            SDL_RenderFillRect(a->renderer, &rect);
        }
    }
}
