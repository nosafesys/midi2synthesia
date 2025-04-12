#include "app.h"
#include "main.h"
#include "midi.h"
#include "input.h"

int main(int argc, char *argv[])
{
    App app = {0};

    app.running = true;

    app_init(&app);

    midi_load_file(&app, "data/example_songs/bohemain-rhapsody.mid");

    // app_device_input(&app);

    app_free(&app);

    return 0;
}
