#include "render.h"
#include "app.h"
#include "note.h"

void render_white_notes(App *a)
{
    for (int i = 0; i < a->note_count; i++)
    {
        if (a->notes[i].height == 0 || a->notes[i].black)
            continue;

        int x = a->notes[i].index * (WHITE_KEY_WIDTH + 2) + 25;
        SDL_Rect rect = {x, a->notes[i].y, WHITE_KEY_WIDTH, a->notes[i].height};
        roundedBoxRGBA(a->renderer, rect.x, rect.y, rect.x + rect.w, rect.y + rect.h,
                       RADIUS, 0, 255, 100, 255);
    }
}

void render_black_notes(App *a)
{
    for (int i = 0; i < a->note_count; i++)
    {
        if (a->notes[i].height == 0 || !a->notes[i].black)
            continue;

        int shifted_white_index = note_white_index(a->notes[i].midi_note) - 1;
        int x = shifted_white_index * (WHITE_KEY_WIDTH + 2) + 25 + WHITE_KEY_WIDTH - (BLACK_KEY_WIDTH / 2);
        SDL_Rect rect = {x, a->notes[i].y, BLACK_KEY_WIDTH, a->notes[i].height};
        roundedBoxRGBA(a->renderer, rect.x, rect.y, rect.x + rect.w, rect.y + rect.h,
                       RADIUS, 0, 120, 40, 255);
    }
}

void render_white_keys(App *a)
{
    for (int i = MIDI_NOTE_MIN; i <= MIDI_NOTE_MAX; i++)
    {
        if (note_is_white(i))
        {
            int index = note_white_index(i);
            SDL_FRect rect = {
                .x = index * (WHITE_KEY_WIDTH + 2) + 25,
                .y = SCREEN_HEIGHT_LARGE - WHITE_KEY_HEIGHT,
                .w = WHITE_KEY_WIDTH,
                .h = WHITE_KEY_HEIGHT};

            if (a->key_active[i])
            {
                SDL_SetRenderDrawColor(a->renderer, 0, 255, 100, 255);
            }
            else
            {
                SDL_SetRenderDrawColor(a->renderer, 255, 255, 255, 255);
            }
            SDL_RenderFillRect(a->renderer, &rect);
        }
    }
}

void render_black_keys(App *a)
{
    for (int i = MIDI_NOTE_MIN; i <= MIDI_NOTE_MAX; i++)
    {
        if (note_is_black(i))
        {
            int shifted_white_index = note_white_index(i) - 1;
            int x = shifted_white_index * (WHITE_KEY_WIDTH + 2) + 25 + WHITE_KEY_WIDTH - (BLACK_KEY_WIDTH / 2);

            SDL_FRect rect = {
                .x = x,
                .y = SCREEN_HEIGHT_LARGE - WHITE_KEY_HEIGHT,
                .w = BLACK_KEY_WIDTH,
                .h = BLACK_KEY_HEIGHT};

            if (a->key_active[i])
            {
                SDL_SetRenderDrawColor(a->renderer, 0, 120, 40, 255);
            }
            else
            {
                SDL_SetRenderDrawColor(a->renderer, 0, 0, 0, 255);
            }
            SDL_RenderFillRect(a->renderer, &rect);
        }
    }
}
