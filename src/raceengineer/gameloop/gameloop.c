#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <poll.h>
#include <termios.h>

#include "gameloop.h"
#include "metricmapper.h"
#include "../helper/parameters.h"
#include "../helper/confighelper.h"
#include "../simulatorapi/simapi/simapi/simapi.h"
#include "../simulatorapi/simapi/simapi/simdata.h"
#include "../simulatorapi/simapi/simapi/simmapper.h"
#include "../simulatorapi/simapi/include/acdata.h"
#include "../simulatorapi/simapi/simmap/mapacdata.h"
#include "../slog/slog.h"

#include "../playwav.h"

#define DEFAULT_UPDATE_RATE      1.0

int looper (RaceEngineerSettings* rs, Metric* metrics, int nummetrics)
{

    slogt("Starting RaceEngineer loop");

    SimData* simdata = malloc(sizeof(SimData));
    SimMap* simmap = malloc(sizeof(SimMap));

    int error = siminit(simdata, simmap, rs->sim_name);

    if (error != RACEENGINEER_ERROR_NONE)
    {
        return error;
    }
    
    simdatamap(simdata, simmap, rs->sim_name);
    engineer_data_init(rs, metrics, nummetrics, simmap);
    slogt("finished setting up user requested data");
    struct termios newsettings, canonicalmode;
    tcgetattr(0, &canonicalmode);
    newsettings = canonicalmode;
    newsettings.c_lflag &= (~ICANON & ~ECHO);
    newsettings.c_cc[VMIN] = 1;
    newsettings.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &newsettings);
    char ch;
    struct pollfd mypoll = { STDIN_FILENO, POLLIN|POLLPRI };

    double update_rate = DEFAULT_UPDATE_RATE;
    int t = 0;
    int nt = 0;
    int lt = 0;
    int go = true;
    int lastlap = 0;

    const char* radiocheckfile = "voice/radio_check/test/1.wav";
    char* temp = malloc(1 + strlen(radiocheckfile) + strlen(rs->sounds_path));
    strcpy(temp, rs->sounds_path);
    strcat(temp, radiocheckfile);
    play(temp);
    free(temp);

    slogt("All data is setup, inside true gameloop");
    while (go == true)
    {
        simdatamap(simdata, simmap, rs->sim_name);

        t = (int) time(NULL);

        int lap = simdata->lap;
        if (lap != lastlap)
        {
            lastlap = lap;
        }
        int j = 0;
        while (j<nummetrics)
        {
            if (metrics[j].enabled == true)
            {
                metrics[j].eval(&metrics[j], lap);
            }
            j++;
        }
        if( poll(&mypoll, 1, 1000.0/update_rate) )
        {
            scanf("%c", &ch);
            if(ch == 'q')
            {
                go = false;
            }
        }
    }

    fprintf(stdout, "\n");
    fflush(stdout);
    
    tcsetattr(0, TCSANOW, &canonicalmode);

    free(simdata);
    free(simmap);

    return 0;
}
