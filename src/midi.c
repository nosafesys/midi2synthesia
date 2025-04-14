#include "midi.h"
#include "app.h"

static int compare_notes(const void *a, const void *b)
{
    const Note *note_a = (const Note *)a;
    const Note *note_b = (const Note *)b;

    if (note_a->start_t < note_b->start_t)
        return -1;
    if (note_a->start_t > note_b->start_t)
        return 1;
    return 0;
}

bool midi_device_count(App *a)
{
    int dev_c = Pm_CountDevices();
    if (dev_c <= 0)
    {
        return false;
    }
    else
    {
        a->dev_c = dev_c;
    }

    return true;
}

bool midi_device_info(App *a, int dev_c)
{

    DeviceInfo *devs = (DeviceInfo *)malloc(dev_c * sizeof(DeviceInfo));

    if (devs == NULL)
    {
        return false;
    }

    for (int i = 0; i < dev_c; i++)
    {
        const PmDeviceInfo *pm_inf = Pm_GetDeviceInfo(i);

        devs[i].name = pm_inf->name;
        devs[i].inp = pm_inf->input;
    }

    a->devs = devs;
    a->dev_c = dev_c;

    return true;
}

bool midi_open_stream(App *a, int dev_id)
{
    PortMidiStream *stream;
    if (Pm_OpenInput(&stream, dev_id, NULL, 512, NULL, NULL) != pmNoError)
    {
        return false;
    }
    else
    {
        a->stream = stream;
        return true;
    }
}

bool midi_load_file(App *a, char *file)
{
    smf_event_t *smf_evt;

    a->song = smf_load(file);
    if (a->song == NULL)
    {
        return false;
    }

    while ((smf_evt = smf_get_next_event(a->song)) != NULL)
    {
        if (smf_event_is_metadata(smf_evt))
            continue;

        uint8_t *buf = smf_evt->midi_buffer;
        uint8_t stat = buf[0];

        if (((stat & 0xF0) == 0x90 && (stat & 0x0F) == 0) ||
            ((stat & 0xF0) == 0x80 && (stat & 0x0F) == 0))
        {
            a->evt_c++;
        }
    }

    a->smf_evts = (smf_event_t **)malloc(a->evt_c * sizeof(smf_event_t *));

    smf_rewind(a->song);

    int idx = 0;
    while ((smf_evt = smf_get_next_event(a->song)) != NULL)
    {
        if (smf_event_is_metadata(smf_evt))
            continue;

        uint8_t *buf = smf_evt->midi_buffer;
        uint8_t stat = buf[0];

        if (((stat & 0xF0) == 0x90 && (stat & 0x0F) == 0) || // Add checks for note offs that are note on w/vel 0
            ((stat & 0xF0) == 0x80 && (stat & 0x0F) == 0))
        {
            a->smf_evts[idx] = smf_evt;
            idx++;
        }
    }

    return true;
}

void midi_poll_events(App *a)
{
    if (Pm_Poll(a->stream))
    {
        PmEvent buf[MAX_MIDI_EVENTS];
        int count = Pm_Read(a->stream, buf, MAX_MIDI_EVENTS);
        for (int i = 0; i < count; i++)
        {
            int st = Pm_MessageStatus(buf[i].message);
            int md_note = Pm_MessageData1(buf[i].message);
            int vel = Pm_MessageData2(buf[i].message);

            if ((st & 0xF0) == 0x90 && vel > 0)
            {
                if (a->note_c < MAX_NOTES)
                {
                    Note note;
                    note.on = true;
                    note.vel = vel;
                    note.y = SCREEN_HEIGHT_LARGE - WHITE_KEY_HEIGHT;
                    note.black = note_is_black(md_note);
                    note.h = MIN_NOTE_HEIGHT;
                    note.idx = note.black ? note_black_index(md_note)
                                          : note_white_index(md_note);
                    note.md_note = md_note;

                    a->notes[a->note_c] = note;
                    a->note_c++;
                }
                a->keys_on[md_note] = true;
            }
            else if ((st & 0xF0) == 0x80 || (((st & 0xF0) == 0x90) && vel == 0))
            {
                for (int j = 0; j < a->note_c; j++)
                {
                    if (a->notes[j].md_note == md_note && a->notes[j].on)
                    {
                        a->notes[j].on = false;
                    }
                }
                a->keys_on[md_note] = false;
            }
        }
    }
}

void midi_get_notes(App *a)
{
    smf_event_t *smf_evt;
    double note_on_times[128] = {0};
    double notes_on[128] = {0};
    a->note_c = 0;
    a->notes = (Note *)malloc((a->evt_c / 2) * sizeof(Note));

    for (int i = 0; i < a->evt_c; i++)
    {
        smf_evt = a->smf_evts[i];
        uint8_t *buf = smf_evt->midi_buffer;
        uint8_t stat = buf[0];
        uint8_t md_note = buf[1];
        uint8_t vel = buf[2];

        if (((stat & 0xF0) == 0x90 && (stat & 0x0F) == 0))
        {
            note_on_times[md_note] = smf_evt->time_seconds;
            notes_on[md_note] = true;
        }
        else if ((stat & 0xF0) == 0x80 && (stat & 0x0F) == 0)
        {
            if (!notes_on[md_note])
            {
                continue;
            }

            double start_t = note_on_times[md_note];
            double end_t = smf_evt->time_seconds;
            double dur = end_t - start_t;

            Note note = {0};
            note.md_note = md_note;
            note.start_t = start_t;
            note.end_t = end_t;
            note.on = false;
            note.h = MIN_NOTE_HEIGHT + (dur * 3);
            note.y = -note.h - 2;
            note.black = note_is_black(md_note);
            note.idx = note.black
                           ? note_black_index(md_note)
                           : note_white_index(md_note);
            note.vel = vel;

            a->notes[a->note_c] = note;
            notes_on[md_note] = false;
            a->note_c++;
        }
    }

    qsort(a->notes, a->note_c, sizeof(Note), compare_notes);
}