#include "app.h"
#include "midi.h"
#include "input.h"

bool app_init(App *a)
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

    a->window = SDL_CreateWindow("midi2synthesia", SCREEN_WIDTH_SMALL, SCREEN_HEIGHT_SMALL, 0);
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

bool app_file_input(App *a)
{
    return true;
}

bool app_device_input(App *a)
{
    SDL_SetWindowFullscreen(a->window, true);

    midi_device_count(a);
    int device_count = a->device_count;
    midi_device_info(a, device_count);

    for (int i = 0; i < device_count; i++)
    {
        DeviceInfo *info = &a->devices[i];
        char *name = info->name;
        int input_support = info->input_support;

        if (input_support)
        {
            printf("Midi device %d: %s [SUPPORTED]\n", i, name);
        }
        else
        {
            printf("Midi device %d: %s [NOT SUPPORTED]\n", i, name);
        }
    }

    int device_id;
    printf("Select a MIDI device ID: ");
    scanf("%d", &device_id);
    if (device_id < 0 || device_id >= device_count)
    {
        fprintf(stderr, "Error: Invalid device ID\n");
        app_free(a);
        return false;
    }

    midi_open_stream(a, device_id);

    main_input_loop(a);

    return true;
}

bool app_free(App *a)
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