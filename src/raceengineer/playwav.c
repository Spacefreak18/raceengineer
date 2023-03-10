#include <ao/ao.h>
#include <sndfile.h>
#include <signal.h>

#include "slog/slog.h"

#define BUFFER_SIZE 8192

int cancel_playback;

void on_cancel_playback(int sig) {
    if (sig != SIGINT) {
        return;
    }

    cancel_playback = 1;
    //exit(0);
}

static void clean(ao_device *device, SNDFILE *file) {
    ao_close(device);
    sf_close(file);
    ao_shutdown();
}

int play(const char* filename) {
    ao_device *device;
    ao_sample_format format;
    SF_INFO sfinfo;

    int default_driver;

    short *buffer;

    signal(SIGINT, on_cancel_playback);


    SNDFILE *file = sf_open(filename, SFM_READ, &sfinfo);

    slogt("Samples: %d", sfinfo.frames);
    slogt("Sample rate: %d", sfinfo.samplerate);
    slogt("Channels: %d", sfinfo.channels);

    ao_initialize();

    default_driver = ao_default_driver_id();

    switch (sfinfo.format & SF_FORMAT_SUBMASK) {
        case SF_FORMAT_PCM_16:
            format.bits = 16;
            break;
        case SF_FORMAT_PCM_24:
            format.bits = 24;
            break;
        case SF_FORMAT_PCM_32:
            format.bits = 32;
            break;
        case SF_FORMAT_PCM_S8:
            format.bits = 8;
            break;
        case SF_FORMAT_PCM_U8:
            format.bits = 8;
            break;
        default:
            format.bits = 16;
            break;
    }

    format.channels = sfinfo.channels;
    format.rate = sfinfo.samplerate;
    format.byte_format = AO_FMT_NATIVE;
    format.matrix = 0;

    device = ao_open_live(default_driver, &format, NULL);

    if (device == NULL) {
        sloge("Error opening device.");
        return 1;
    }

    buffer = calloc(BUFFER_SIZE, sizeof(short));

    while (1) {
        int read = sf_read_short(file, buffer, BUFFER_SIZE);

        if (ao_play(device, (char *) buffer, (uint_32) (read * sizeof(short))) == 0) {
            //sloge("ao_play: failed.");
            //clean(device, file);
            break;
        }

        if (cancel_playback) {
            //clean(device, file);
            break;
        }
    }

    clean(device, file);
    free(buffer);

    return 0;
}
