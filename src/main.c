#include "app.h"
#include "main.h"
#include "midi.h"
#include "input.h"

int main(int argc, char *argv[])
{
    App app;

    app.running = true;

    app_init(&app);

    app_device_input(&app);

    app_free(&app);

    return 0;
}
