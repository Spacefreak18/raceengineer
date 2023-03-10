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

#include "../simulatorapi/simapi/include/rf2data.h"
#include "../simulatorapi/simapi/simmap/maprf2data.h"

#include "../slog/slog.h"



int rf2_init(struct Map* map, RF2Map *rf2map, int mapdata)
{
    slogd("Mapping RFactor2 data");
    CreateRF2Map(map, rf2map, mapdata);
    return 0;
}

int ac_init(struct Map* map, ACMap *acmap, int mapdata)
{
    slogd("Mapping Assetto Corsa data");
    CreateACMap(map, acmap, mapdata);
    return 0;
}

int get_map_size(RaceEngineerSettings* rs)
{
    switch ( rs->sim_name )
    {
        case SIMULATOR_ASSETTO_CORSA :
            return ACMAP_SIZE;
        case SIMULATOR_RFACTOR2 :
            return RF2MAP_SIZE;
    }
}

struct Map* create_map(RaceEngineerSettings* rs, SimMap* simmap, int mapdata)
{
    struct Map* map;
    switch ( rs->sim_name )
    {
        case SIMULATOR_ASSETTO_CORSA :
            map = (struct Map*) malloc((ACMAP_SIZE) * sizeof(struct Map));
            ac_init(map, &simmap->d.ac, mapdata);
            break;
        case SIMULATOR_RFACTOR2 :
            map = (struct Map*) malloc((RF2MAP_SIZE) * sizeof(struct Map));
            rf2_init(map, &simmap->d.rf2, mapdata);
            break;
    }
    return map;
}

int engineer_data_init(RaceEngineerSettings* rs, Metric* metrics, int nummetrics, SimMap* simmap)
{
    struct Map* map = create_map(rs, simmap, 1);

    slogt("Matching %i configured metrics with available data", nummetrics);
    for(int i = 0; i < nummetrics; i++)
    {
        if (metrics[i].variable == NULL)
        {
            continue;
        }
        slogt("Checking %s", metrics[i].name);
        for (int k = 0; k < get_map_size(rs); k++)
        {
            slogt("Matching %s to %s", map[k].name, metrics[i].variable);
            if (map[k].name == NULL || metrics[i].variable == NULL)
            {
                continue;
            }
            if (strcmp(map[k].name, metrics[i].variable) == 0)
            {
                metrics[i].value = map[k].value;
                //switch(metrics[i].type)
                //{
                //    case SINGLEINTEGER:
                //        IntegerMetric* im = (void *) metrics[i].derived;
                //        im->value = map[k].value;
                //        break;
                //    case SINGLEFLOAT:
                //        FloatMetric* fm = (void *) metrics[i].derived;
                //        fm->value = map[k].value;
                //        break;
                //    case SINGLEDOUBLE:
                //        DoubleMetric* dm = (void *) metrics[i].derived;
                //        dm->value = map[k].value;
                //        break;
                //}
                slogd("Match found!!");
                break;
            }
        }
    }
    free(map);
    return 0;
}

