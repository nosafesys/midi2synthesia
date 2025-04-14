#include "app.h"
#include "midi.h"
#include "input.h"

bool app_init(App *a)
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS))
    {
        return false;
    }

    if (!(TTF_Init()))
    {
        return false;
    }

    a->window = SDL_CreateWindow("midi2synthesia", SCREEN_WIDTH_SMALL, SCREEN_HEIGHT_SMALL, 0);
    if (!a->window)
    {
        return false;
    }

    a->renderer = SDL_CreateRenderer(a->window, NULL);
    if (!a->renderer)
    {
        return false;
    }

    PmError result = Pm_Initialize();
    if (result != pmNoError)
    {
        return false;
    }

    return true;
}

void app_file_input(App *a, char *file)
{
    SDL_SetWindowFullscreen(a->window, true);
    midi_load_file(a, file);

    input_loop_f(a);
}

void app_device_input(App *a)
{
    SDL_SetWindowFullscreen(a->window, true);

    midi_device_count(a);
    int dev_c = a->dev_c;
    midi_device_info(a, dev_c);

    midi_open_stream(a, 3);

    input_loop_d(a);
}

bool app_free(App *a)
{
    smf_delete(a->song);

    PmError pm_res;
    const char *sdl_res;

    free(a->devs);

    pm_res = Pm_Close(a->stream);
    if (pm_res != pmNoError)
    {
        return false;
    }

    pm_res = Pm_Terminate();
    if (pm_res != pmNoError)
    {
        return false;
    }

    TTF_Quit();
    sdl_res = SDL_GetError();
    if (sdl_res && sdl_res[0] != '\0')
    {
        return false;
    }

    SDL_DestroyRenderer(a->renderer);
    sdl_res = SDL_GetError();
    if (sdl_res && sdl_res[0] != '\0')
    {
        return false;
    }

    SDL_DestroyWindow(a->window);
    sdl_res = SDL_GetError();
    if (sdl_res && sdl_res[0] != '\0')
    {
        return false;
    }

    SDL_Quit();
    sdl_res = SDL_GetError();
    if (sdl_res && sdl_res[0] != '\0')
    {
        return false;
    }

    return true;
}