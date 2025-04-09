#include "midi.h"
#include "app.h"
#include "render.h"
#include "note.h"

void main_input_loop(App *a)
{
    memset(a->notes, 0, sizeof(a->notes));
    a->note_count = 0;
    memset(a->key_active, false, sizeof(a->key_active));

    while (a->running)
    {
        while (SDL_PollEvent(&a->event))
        {
            if (a->event.type == SDL_EVENT_QUIT)
                a->running = false;
        }

        midi_poll_events(a);

        // Update note positions:
        for (int i = 0; i < a->note_count; i++)
        {
            if (a->notes[i].active)
            {
                a->notes[i].y -= 3;
                a->notes[i].height += 3;
            }
            else if (a->notes[i].height > 0)
            {
                a->notes[i].y -= 3;

                if (a->notes[i].y + a->notes[i].height < 0)
                {
                    a->notes[i].height = 0;
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
        for (int i = 0; i < a->note_count; i++)
        {
            if (a->notes[i].height > 0)
            {
                a->notes[j++] = a->notes[i];
            }
        }
        a->note_count = j;
    }
}
