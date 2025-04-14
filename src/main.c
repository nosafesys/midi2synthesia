/*
 * Required header files for application functionality
 */
#include "app.h"
#include "main.h"
#include "midi.h"
#include "input.h"

/*
 * Main entry point for the MIDI2Synthesia application
 * Initializes the application, processes MIDI input, and handles cleanup
 *
 * @param argc Number of command line arguments
 * @param argv Array of command line argument strings
 * @return 0 on success, -1 on error
 */
int main(int argc, char **argv)
{
    /* Initialize application structure with zero values */
    App a = {0};

    /* Set initial running state to true */
    a.running = true;

    /* Initialize SDL and other required systems */
    if (!app_init(&a))
    {
        fprintf(stderr, "Initialization error");
        return -1;
    }

    /* Process MIDI file input */
    /* Currently set to use Chopin's Etude Op. 10 No. 4 */
    /* Alternatively can use app_device_input for live MIDI input */
    app_file_input(&a, "data/example_songs/chopin-etude-op10-no4.mid");
    // app_device_input(&a);

    /* Cleanup and free resources */
    if (!app_free(&a))
    {
        fprintf(stderr, "Cleanup error");
        return -1;
    }

    return 0;
}
