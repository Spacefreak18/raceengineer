#include "../helper/parameters.h"
#include "../metric.h"
#include "../simulatorapi/simapi/simapi/simmapper.h"

int get_map_size(RaceEngineerSettings* rs);
struct Map* create_map(RaceEngineerSettings* rs, SimMap* simmap, int mapdata);
int engineer_data_init(RaceEngineerSettings* rs, Metric* metrics, int nummetrics, SimMap* simmap);
