#include "midi.h"
#include "app.h"
#include "render.h"
#include "note.h"
#include "input.h"

void input_loop_d(App *a)
{
    a->notes = (Note *)malloc(MAX_NOTES * sizeof(Note));

    while (a->running)
    {
        while (SDL_PollEvent(&a->sdl_evt))
        {
            if (a->sdl_evt.type == SDL_EVENT_QUIT)
                a->running = false;
        }

        midi_poll_events(a);

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

        SDL_SetRenderDrawColor(a->renderer, 49, 49, 49, 255);
        SDL_RenderClear(a->renderer);

        render_white_notes(a);
        render_black_notes(a);
        render_white_keys(a);
        render_black_keys(a);

        SDL_RenderPresent(a->renderer);
        SDL_Delay(16);

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
}

void input_loop_f(App *a)
{
    double start_time = SDL_GetTicks() / 1000.0;
    double current_time = 0.0;
    int note_idx = 0;

    midi_get_notes(a);

    while (a->running)
    {
        while (SDL_PollEvent(&a->sdl_evt))
        {
            if (a->sdl_evt.type == SDL_EVENT_QUIT)
                a->running = false;
        }

        current_time = (SDL_GetTicks() / 1000.0) - start_time;

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

        SDL_SetRenderDrawColor(a->renderer, 49, 49, 49, 255);
        SDL_RenderClear(a->renderer);

        render_white_notes(a);
        render_black_notes(a);
        render_white_keys(a);
        render_black_keys(a);

        SDL_RenderPresent(a->renderer);
        SDL_Delay(16);
    }
}