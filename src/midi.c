#include "midi.h"
#include "app.h"

bool midi_device_count(App *a)
{
    int device_count = Pm_CountDevices();
    if (device_count <= 0)
    {
        fprintf(stderr, "Error: No MIDI input devices found");
        return false;
    }
    else
    {
        a->device_count = device_count;
    }

    return true;
}

bool midi_device_info(App *a, int device_count)
{

    DeviceInfo *devices = (DeviceInfo *)malloc(device_count * sizeof(DeviceInfo));

    if (devices == NULL)
    {
        fprintf(stderr, "Error: Unable to allocate memory for device data");
        return false;
    }

    for (int i = 0; i < device_count; i++)
    {
        const PmDeviceInfo *pminfo = Pm_GetDeviceInfo(i);

        devices[i].name = pminfo->name;
        devices[i].input_support = pminfo->input;
    }

    a->devices = devices;
    a->device_count = device_count;

    return true;
}

bool midi_open_stream(App *a, int device_id)
{
    PortMidiStream *stream;
    if (Pm_OpenInput(&stream, device_id, NULL, 512, NULL, NULL) != pmNoError)
    {
        fprintf(stderr, "Error: Failed to open MIDI stream\n");
        return false;
    }
    else
    {
        a->stream = stream;
        return true;
    }
}

bool midi_load_file(App *a, const char *filename)
{
    smf_event_t *event;
    char *event_text;

    a->smf_song = smf_load(filename);
    if (a->smf_song == NULL)
    {
        fprintf(stderr, "Error: Failed to load MIDI file\n");
        return false;
    }

    while ((event = smf_get_next_event(a->smf_song)) != NULL)
    {
        if (smf_event_is_metadata(event))
            continue;

        uint8_t *buffer = event->midi_buffer;
        uint8_t status = buffer[0];
        uint8_t data = buffer[1];

        if (status == 0x90)
        {
            a->events[a->event_count] = event;
            a->event_count++;
        }
        else if (status == 0x80)
        {
            a->event_count++;
        }
    }

    smf_delete(a->smf_song);
    return true;
}

void midi_poll_events(App *a)
{
    if (Pm_Poll(a->stream))
    {
        PmEvent buffer[MAX_MIDI_EVENTS];
        int count = Pm_Read(a->stream, buffer, MAX_MIDI_EVENTS);
        for (int i = 0; i < count; i++)
        {
            int status = Pm_MessageStatus(buffer[i].message);
            int midi_note = Pm_MessageData1(buffer[i].message);
            int velocity = Pm_MessageData2(buffer[i].message);

            if ((status & 0xF0) == 0x90 && velocity > 0)
            {
                if (a->note_count < MAX_NOTES)
                {
                    Note note;
                    note.active = true;
                    note.velocity = velocity;
                    note.y = SCREEN_HEIGHT_LARGE - WHITE_KEY_HEIGHT;
                    note.black = note_is_black(midi_note);
                    note.height = MIN_NOTE_HEIGHT;
                    note.index = note.black ? note_black_index(midi_note)
                                            : note_white_index(midi_note);
                    note.midi_note = midi_note;

                    a->notes[a->note_count] = note;
                    a->note_count++;
                }
                a->key_active[midi_note] = true;
            }
            else if ((status & 0xF0) == 0x80 || (((status & 0xF0) == 0x90) && velocity == 0))
            {
                for (int j = 0; j < a->note_count; j++)
                {
                    if (a->notes[j].midi_note == midi_note && a->notes[j].active)
                    {
                        a->notes[j].active = false;
                    }
                }
                a->key_active[midi_note] = false;
            }
        }
    }
}
