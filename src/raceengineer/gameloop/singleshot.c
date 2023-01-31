#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

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


int singleshot(RaceEngineerSettings* rs, Metric* metrics[], int nummetrics)
{

    slogt("Starting RaceEngineer in Singleshot mode");

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


    slogt("All data is setup, inside true gameloop");

    simdatamap(simdata, simmap, rs->sim_name);

    int j = 0;
    slogt("Looking for metric for data node: %s", rs->metricname);
    while (j<nummetrics)
    {

        if (metrics[j]->enabled == true && strcmp(metrics[j]->name, rs->metricname) == 0)
        {
            slogt("Evaluating metric %s", metrics[j]->name);
            metrics[j]->eval(metrics[j], -1);
        }
        j++;
    }

    free(simdata);
    free(simmap);

    return 0;
}
