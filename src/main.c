#include "app.h"
#include "main.h"
#include "init.h"
#include "midi.h"
#include "cleanup.h"
#include "input.h"

int main()
{
    App app;

    app.running = true;

    init(&app);

    midi_device_count(&app);
    int device_count = app.device_count;
    midi_device_info(&app, device_count);

    for (int i = 0; i < device_count; i++)
    {
        DeviceInfo *info = &app.devices[i];
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
        cleanup(&app);
        return 1;
    }

    midi_open_stream(&app, device_id);

    main_input_loop(&app);

    cleanup(&app);

    return 0;
}
