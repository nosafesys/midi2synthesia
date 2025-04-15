/*
 * Application-specific implementation
 * Handles initialization, cleanup, and input processing
 */

#include "app.h"
#include "midi.h"
#include "input.h"

/*
 * Initialize the application structure and SDL systems
 * Returns true on success, false on failure
 */
bool app_init(App *a)
{
    // Initialize SDL with video and events subsystems
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS))
    {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return false;
    }

    // Initialize SDL_ttf for font rendering
    if (!(TTF_Init()))
    {
        return false;
    }

    // Create the SDL window with specified dimensions
    a->window = SDL_CreateWindow("midi2synthesia", SCREEN_WIDTH_SMALL, SCREEN_HEIGHT_SMALL, 0);
    if (!a->window)
    {
        fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
        return false;
    }

    // Create the SDL renderer for rendering graphics
    a->renderer = SDL_CreateRenderer(a->window, NULL);
    if (!a->renderer)
    {
        fprintf(stderr, "Renderer creation failed: %s\n", SDL_GetError());
        return false;
    }

    // Initialize PortMidi library
    PmError result = Pm_Initialize();
    if (result != pmNoError)
    {
        return false;
    }

    return true;
}

/*
 * Process MIDI input from a file
 * Loads the file and extracts note information
 */
void app_file_input(App *a, char *file)
{
    // Set the application window to fullscreen mode
    SDL_SetWindowFullscreen(a->window, true);

    // Load the MIDI file and extract events
    midi_load_file(a, file);

    // Enter the input loop for file-based MIDI playback
    input_loop_f(a);
}

/*
 * Process MIDI input from a device
 * Reads events and updates the application state
 */
void app_device_input(App *a)
{
    // Set the application window to fullscreen mode
    SDL_SetWindowFullscreen(a->window, true);

    // Count available MIDI devices
    midi_device_count(a);
    int dev_c = a->dev_c;

    // Retrieve information about MIDI devices
    midi_device_info(a, dev_c);

    // Open a MIDI input stream for a specific device (hardcoded to device 3)
    midi_open_stream(a, 3);

    // Enter the input loop for device-based MIDI playback
    input_loop_d(a);
}

/*
 * Free resources and clean up the application structure
 * Returns true on success, false on failure
 */
bool app_free(App *a)
{
    // Delete the loaded MIDI song
    smf_delete(a->song);

    PmError pm_res;
    const char *sdl_res;

    // Free allocated memory for device information
    free(a->devs);

    // Close the PortMidi stream
    pm_res = Pm_Close(a->stream);
    if (pm_res != pmNoError)
    {
        return false;
    }

    // Terminate the PortMidi library
    pm_res = Pm_Terminate();
    if (pm_res != pmNoError)
    {
        return false;
    }

    // Quit SDL_ttf library
    TTF_Quit();
    sdl_res = SDL_GetError();
    if (sdl_res && sdl_res[0] != '\0')
    {
        return false;
    }

    // Destroy the SDL renderer
    SDL_DestroyRenderer(a->renderer);
    sdl_res = SDL_GetError();
    if (sdl_res && sdl_res[0] != '\0')
    {
        return false;
    }

    // Destroy the SDL window
    SDL_DestroyWindow(a->window);
    sdl_res = SDL_GetError();
    if (sdl_res && sdl_res[0] != '\0')
    {
        return false;
    }

    // Quit SDL subsystems
    SDL_Quit();
    sdl_res = SDL_GetError();
    if (sdl_res && sdl_res[0] != '\0')
    {
        return false;
    }

    return true;
}