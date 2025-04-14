#include "app.h"
#include "main.h"
#include "midi.h"
#include "input.h"

int main(int argc, char **argv)
{
    App a = {0};

    a.running = true;

    if (!app_init(&a))
    {
        fprintf(stderr, "Initialization error");
        return -1;
    }

    app_file_input(&a, "data/example_songs/bohemain-rhapsody.mid");
    // app_device_input(&a);

    if (!app_free(&a))
    {
        fprintf(stderr, "Cleanup error");
        return -1;
    }

    return 0;
}
