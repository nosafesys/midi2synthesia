#include "cleanup.h"
#include "app.h"

bool cleanup(App *a)
{
    PmError pm_result;
    const char *sdl_result;

    free(a->devices);

    pm_result = Pm_Close(a->stream);
    if (pm_result != pmNoError)
    {
        fprintf(stderr, "Error: %s\n", Pm_GetErrorText(pm_result));
        return false;
    }

    pm_result = Pm_Terminate();
    if (pm_result != pmNoError)
    {
        fprintf(stderr, "Error: %s\n", Pm_GetErrorText(pm_result));
        return false;
    }

    TTF_Quit();
    sdl_result = SDL_GetError();
    if (sdl_result && sdl_result[0] != '\0')
    {
        fprintf(stderr, "Error: %s\n", sdl_result);
        return false;
    }

    SDL_DestroyRenderer(a->renderer);
    sdl_result = SDL_GetError();
    if (sdl_result && sdl_result[0] != '\0')
    {
        fprintf(stderr, "Error: %s\n", sdl_result);
        return false;
    }

    SDL_DestroyWindow(a->window);
    sdl_result = SDL_GetError();
    if (sdl_result && sdl_result[0] != '\0')
    {
        fprintf(stderr, "Error: %s\n", sdl_result);
        return false;
    }

    SDL_Quit();
    sdl_result = SDL_GetError();
    if (sdl_result && sdl_result[0] != '\0')
    {
        fprintf(stderr, "Error: %s\n", sdl_result);
        return false;
    }

    return true;
}