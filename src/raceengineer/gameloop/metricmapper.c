#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gameloop.h"
#include "../helper/parameters.h"
#include "../helper/confighelper.h"
#include "../simulatorapi/simapi/simapi/simapi.h"
#include "../simulatorapi/simapi/simapi/simdata.h"
#include "../simulatorapi/simapi/simapi/simmapper.h"
#include "../simulatorapi/simapi/include/acdata.h"
#include "../simulatorapi/simapi/simmap/mapacdata.h"
#include "../slog/slog.h"



int rf2_init()
{
    return 0;
}

int ac_init(struct Map* map, ACMap *acmap)
{
    slogd("Mapping Assetto Corsa data");
    CreateACMap(map, acmap);
    return 0;
}

int engineer_data_init(RaceEngineerSettings* rs, Metric* metrics[], int nummetrics, SimMap* simmap)
{

    // need a switch to set map size accordingly and run correct map function
    struct Map* map = (struct Map*) malloc((ACMAP_SIZE) * sizeof(struct Map));
    switch ( rs->sim_name )
    {
        case SIMULATOR_ASSETTO_CORSA :
            ac_init(map, &simmap->d.ac);
            break;
    }

    slogt("Matching %i configured metrics with available data", nummetrics);
    for(int i = 0; i < nummetrics; i++)
    {
        if (metrics[i]->variable == NULL)
        {
            continue;
        }
        slogt("Checking %s", metrics[i]->name);
        for (int k = 0; k < ACMAP_SIZE; k++)
        {
            slogt("Matching %s to %s", map[k].name, metrics[i]->variable);
            if (map[k].name == NULL || metrics[i]->variable == NULL)
            {
                continue;
            }
            if (strcmp(map[k].name, metrics[i]->variable) == 0)
            {

                switch(metrics[i]->type) {
                    case INTEGER:
                        IntegerMetric* im = (void *) metrics[i]->derived;
                        im->value = map[k].value;
                        break;
                    case FLOAT:
                        FloatMetric* fm = (void *) metrics[i]->derived;
                        fm->value = map[k].value;
                        break;
                    case DOUBLE:
                        DoubleMetric* dm = (void *) metrics[i]->derived;
                        dm->value = map[k].value;
                        break;
                }
                slogd("Match found!!");
                break;
            }
        }
    }
    free(map);
    return 0;
}

