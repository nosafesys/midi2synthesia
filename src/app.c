/*
 * Application-specific implementation
 * Handles initialization, cleanup, and input processing
 */

#include "app.h"
#include "midi.h"
#include "input.h"

/*
 * Initialize SDL systems, PortMidi, fluidsynth
 * Returns true on success, false on failure
 */
bool app_init(App *a)
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS))
    {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return false;
    }

    if (!(TTF_Init()))
    {
        return false;
    }

    a->window = SDL_CreateWindow("midi2synthesia", SCREEN_WIDTH_SMALL, SCREEN_HEIGHT_SMALL, 0);
    if (!a->window)
    {
        fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
        return false;
    }

    a->renderer = SDL_CreateRenderer(a->window, NULL);
    if (!a->renderer)
    {
        fprintf(stderr, "Renderer creation failed: %s\n", SDL_GetError());
        return false;
    }

    PmError result = Pm_Initialize();
    if (result != pmNoError)
    {
        return false;
    }

    a->settings = new_fluid_settings();
    if (a->settings == NULL)
    {
        return false;
    }

    a->synth = new_fluid_synth(a->settings);
    if (a->synth == NULL)
    {
        return false;
    }

    if (fluid_synth_sfload(a->synth, "data/sounds/soundfont.sf2", 1))
    {
        return false;
    }

    a->adriver = new_fluid_audio_driver(a->settings, a->synth);
    if (a->adriver == NULL)
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

    // Open a MIDI input stream for a specific device
    midi_open_stream(a, 3);

    // Enter the input loop for device-based MIDI playback
    input_loop_d(a);
}

/*
 * Free resources and clean up the application structure
 * Returns true on success, false on failure
 */
void app_free(App *a)
{
    if (a->song)
    {
        smf_delete(a->song);
    }

    if (a->notes)
    {
        free(a->notes);
    }

    if (a->devs)
    {
        free(a->devs);
    }

    if (a->adriver)
    {
        delete_fluid_audio_driver(a->adriver);
    }

    if (a->synth)
    {
        delete_fluid_synth(a->synth);
    }

    if (a->settings)
    {
        delete_fluid_settings(a->settings);
    }

    if (a->stream)
    {
        Pm_Close(a->stream);
    }

    Pm_Terminate();

    TTF_Quit();

    if (a->renderer)
    {
        SDL_DestroyRenderer(a->renderer);
    }

    if (a->window)
    {
        SDL_DestroyWindow(a->window);
    }

    SDL_Quit();
}