/*
 * MIDI processing implementation
 * Handles MIDI file loading, event polling, and note extraction
 */

#include "midi.h"
#include "app.h"

/*
 * Compare two notes based on their start time
 * Used for sorting notes in ascending order of start time
 */
static int compare_notes(const void *a, const void *b)
{
    const Note *note_a = (const Note *)a;
    const Note *note_b = (const Note *)b;

    // Compare start times of the notes
    if (note_a->start_t < note_b->start_t)
        return -1;
    if (note_a->start_t > note_b->start_t)
        return 1;
    return 0;
}

/*
 * Count the number of available MIDI devices
 * Updates the application structure with the device count
 * Returns true if devices are found, false otherwise
 */
bool midi_device_count(App *a)
{
    // Get the number of MIDI devices
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

/*
 * Retrieve information about available MIDI devices
 * Allocates memory for device information and updates the application structure
 * Returns true on success, false on failure
 */
bool midi_device_info(App *a, int dev_c)
{
    // Allocate memory for device information
    DeviceInfo *devs = (DeviceInfo *)malloc(dev_c * sizeof(DeviceInfo));

    if (devs == NULL)
    {
        return false;
    }

    // Populate device information
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

/*
 * Open a MIDI input stream for the specified device ID
 * Updates the application structure with the stream handle
 * Returns true on success, false on failure
 */
bool midi_open_stream(App *a, int dev_id)
{
    PortMidiStream *stream;
    // Open the MIDI input stream
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

/*
 * Load a MIDI file and extract relevant events
 * Updates the application structure with the loaded song and events
 * Returns true on success, false on failure
 */
bool midi_load_file(App *a, char *file)
{
    smf_event_t *smf_evt;

    // Load the MIDI file
    a->song = smf_load(file);
    if (a->song == NULL)
    {
        return false;
    }

    // Count the number of relevant MIDI events
    while ((smf_evt = smf_get_next_event(a->song)) != NULL)
    {
        if (smf_event_is_metadata(smf_evt))
            continue;

        uint8_t *buf = smf_evt->midi_buffer;
        uint8_t stat = buf[0];

        // Check for note-on and note-off events
        if (((stat & 0xF0) == 0x90 && (stat & 0x0F) == 0) ||
            ((stat & 0xF0) == 0x80 && (stat & 0x0F) == 0))
        {
            a->evt_c++;
        }
    }

    // Allocate memory for storing MIDI events
    a->smf_evts = (smf_event_t **)malloc(a->evt_c * sizeof(smf_event_t *));

    // Rewind the MIDI file and store relevant events
    smf_rewind(a->song);

    int idx = 0;
    while ((smf_evt = smf_get_next_event(a->song)) != NULL)
    {
        if (smf_event_is_metadata(smf_evt))
            continue;

        uint8_t *buf = smf_evt->midi_buffer;
        uint8_t stat = buf[0];

        // Check for note-on and note-off events
        if (((stat & 0xF0) == 0x90 && (stat & 0x0F) == 0) ||
            ((stat & 0xF0) == 0x80 && (stat & 0x0F) == 0))
        {
            a->smf_evts[idx] = smf_evt;
            idx++;
        }
    }

    return true;
}

/*
 * Poll for MIDI events from the input stream
 * Updates the application structure with active notes and key states
 */
void midi_poll_events(App *a)
{
    // Check if there are MIDI events available
    if (Pm_Poll(a->stream))
    {
        PmEvent buf[MAX_MIDI_EVENTS];
        int count = Pm_Read(a->stream, buf, MAX_MIDI_EVENTS);

        // Process each MIDI event
        for (int i = 0; i < count; i++)
        {
            int st = Pm_MessageStatus(buf[i].message);
            int md_note = Pm_MessageData1(buf[i].message);
            int vel = Pm_MessageData2(buf[i].message);

            // Handle note-on events
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
            // Handle note-off events
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

/*
 * Extract notes from the loaded MIDI file
 * Updates the application structure with note information
 */
void midi_get_notes(App *a)
{
    smf_event_t *smf_evt;
    double note_on_times[128] = {0};
    double notes_on[128] = {0};
    a->note_c = 0;

    // Allocate memory for notes
    a->notes = (Note *)malloc((a->evt_c / 2) * sizeof(Note));

    // Process each MIDI event
    for (int i = 0; i < a->evt_c; i++)
    {
        smf_evt = a->smf_evts[i];
        uint8_t *buf = smf_evt->midi_buffer;
        uint8_t stat = buf[0];
        uint8_t md_note = buf[1];
        uint8_t vel = buf[2];

        // Handle note-on events
        if (((stat & 0xF0) == 0x90 && (stat & 0x0F) == 0))
        {
            note_on_times[md_note] = smf_evt->time_seconds;
            notes_on[md_note] = true;
        }
        // Handle note-off events
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

    // Sort notes by start time
    qsort(a->notes, a->note_c, sizeof(Note), compare_notes);
}