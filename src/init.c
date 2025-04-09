#include "app.h"

bool init(App *a)
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS))
    {
        fprintf(stderr, "Error: %s", SDL_GetError());
        return false;
    }

    if (!(TTF_Init()))
    {
        fprintf(stderr, "Error: %s", SDL_GetError());
        return false;
    }

    a->window = SDL_CreateWindow("midi2synthesia", SCREEN_WIDTH_LARGE, SCREEN_HEIGHT_LARGE, SDL_WINDOW_FULLSCREEN);
    if (!a->window)
    {
        fprintf(stderr, "Error: %s", SDL_GetError());
        return false;
    }

    a->renderer = SDL_CreateRenderer(a->window, NULL);
    if (!a->renderer)
    {
        fprintf(stderr, "Error: %s", SDL_GetError());
        return false;
    }

    PmError result = Pm_Initialize();
    if (result != pmNoError)
    {
        fprintf(stderr, "Error: %s", Pm_GetErrorText(result));
        return false;
    }

    return true;
}
