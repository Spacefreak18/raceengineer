#ifndef _CONFIGHELPER_H
#define _CONFIGHELPER_H

#include <stdbool.h>
#include <stdint.h>

#include <libconfig.h>

#include "parameters.h"

#include "../metric.h"

#include "../simulatorapi/simapi/simapi/simapi.h"
#include "../simulatorapi/simapi/simmap/basicmap.h"

typedef enum
{
    SIMDEVTYPE_UNKNOWN           = 0,
    SIMDEVTYPE_TACHOMETER        = 1,
    SIMDEVTYPE_SHAKER            = 2,
    SIMDEVTYPE_SHIFTLIGHTS       = 3
}
DeviceSubType;

typedef enum
{
    RACEENGINEER_ERROR_NONE          = 0,
    RACEENGINEER_ERROR_UNKNOWN       = 1,
    RACEENGINEER_ERROR_INVALID_SIM   = 2,
    RACEENGINEER_ERROR_NODATA        = 3,
    RACEENGINEER_ERROR_BAD_CONFIG    = 4,
}
RaceEngineerError;

typedef struct
{
    ProgramAction program_action;
    Simulator sim_name;
    char* metric_name;
    char* sounds_path;
}
RaceEngineerSettings;

int strtogame(const char* game, RaceEngineerSettings* ms);
int loadconfig_scan(config_t* cfg);
int loadconfig(RaceEngineerSettings* rs, config_t* cfg, Metric* metrics, int nummetrics, struct Map* map, int mapsize);
void resettingsfree(RaceEngineerSettings* rs);

#endif
