/*
 * Input handling implementation
 * Processes user input for both file and device modes
 */

#include "midi.h"
#include "app.h"
#include "render.h"
#include "note.h"
#include "input.h"

/*
 * Handle input events in device mode
 * Updates the application state based on user actions
 */
void input_loop_d(App *a)
{
    // Allocate memory for notes
    a->notes = (Note *)malloc(MAX_NOTES * sizeof(Note));

    // Poll for SDL events
    while (a->running)
    {
        while (SDL_PollEvent(&a->sdl_evt))
        {
            // Handle quit event
            if (a->sdl_evt.type == SDL_EVENT_QUIT)
                a->running = false;
        }

        // Poll for MIDI events
        midi_poll_events(a);

        // Update note positions and heights
        for (int i = 0; i < a->note_c; i++)
        {
            if (a->notes[i].on)
            {
                a->notes[i].y -= 3;
                a->notes[i].h += 3;
            }
            else if (a->notes[i].h > 0)
            {
                a->notes[i].y -= 3;

                if (a->notes[i].y + a->notes[i].h < 0)
                {
                    a->notes[i].h = 0;
                }
            }
        }

        // Clear the screen
        SDL_SetRenderDrawColor(a->renderer, 49, 49, 49, 255);
        SDL_RenderClear(a->renderer);

        // Render notes and keys
        render_white_notes(a);
        render_black_notes(a);
        render_white_keys(a);
        render_black_keys(a);

        // Present the rendered frame
        SDL_RenderPresent(a->renderer);
        SDL_Delay(16);

        // Remove notes that are no longer visible
        int j = 0;
        for (int i = 0; i < a->note_c; i++)
        {
            if (a->notes[i].h > 0)
            {
                a->notes[j++] = a->notes[i];
            }
        }
        a->note_c = j;
    }

    // Free allocated memory for notes
    free(a->notes);
}

/*
 * Handle input events in file mode
 * Updates the application state based on user actions
 */
void input_loop_f(App *a)
{
    // Initialize timing variables
    double start_time = SDL_GetTicks() / 1000.0;
    double current_time = 0.0;
    int note_idx = 0;

    // Extract notes from the MIDI file
    midi_get_notes(a);

    // Poll for SDL events
    while (a->running)
    {
        while (SDL_PollEvent(&a->sdl_evt))
        {
            // Handle quit event
            if (a->sdl_evt.type == SDL_EVENT_QUIT)
                a->running = false;
        }

        // Update the current time
        current_time = (SDL_GetTicks() / 1000.0) - start_time;

        // Activate notes based on their start time
        while (note_idx < a->note_c)
        {
            if (a->notes[note_idx].start_t <= current_time)
            {
                a->notes[note_idx].on = true;
                note_idx++;
            }
            else
            {
                break;
            }
        }

        // Update key states
        bool temp_keys_on[128] = {false};

        for (int i = 0; i < note_idx; i++)
        {
            Note *note = &a->notes[i];

            if (note->y > SCREEN_HEIGHT_LARGE - WHITE_KEY_HEIGHT)
            {
                note->h = 0;
            }

            if (note->y < SCREEN_HEIGHT_LARGE - WHITE_KEY_HEIGHT && note->y + note->h >= SCREEN_HEIGHT_LARGE - WHITE_KEY_HEIGHT)
            {
                temp_keys_on[note->md_note] = true;
            }

            note->y += 3;
        }

        for (int i = 0; i < 128; i++)
        {
            a->keys_on[i] = temp_keys_on[i];
        }

        // Clear the screen
        SDL_SetRenderDrawColor(a->renderer, 49, 49, 49, 255);
        SDL_RenderClear(a->renderer);

        // Render notes and keys
        render_white_notes(a);
        render_black_notes(a);
        render_white_keys(a);
        render_black_keys(a);

        // Present the rendered frame
        SDL_RenderPresent(a->renderer);
        SDL_Delay(16);
    }
}