#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "confighelper.h"

#include "../slog/slog.h"

#include "../metric.h"

#define CONFIGSTR_REPORTTYPE        "reporttype"
#define CONFIGVAL_BASICREPORT       "basic"
#define CONFIGVAL_SUMREPORT         "summary"

#define CONFIGSTR_DATATYPE          "datatype"
#define CONFIGVAL_FLOAT             "float"
#define CONFIGVAL_DOUBLE            "double"
#define CONFIGVAL_INTEGER           "int"

#define CONFIGSTR_MINMAX            "minormax"
#define CONFIGSTR_REPEAT            "repeat"

void resettingsfree(RaceEngineerSettings* rs)
{
    free(rs->sounds_path);
    free(rs);
}

int strtogame(const char* game, RaceEngineerSettings* rs)
{
    slogd("Checking for %s in list of supported simulators.", game);
    if (strcmp(game, "ac") == 0)
    {
        slogd("Setting simulator to Assetto Corsa");
        rs->sim_name = SIMULATOR_ASSETTO_CORSA;
    }
    else if (strcmp(game, "rf2") == 0)
    {
        slogd("Setting simulator to Assetto Corsa");
        rs->sim_name = SIMULATOR_RFACTOR2;
    }
    else
        if (strcmp(game, "test") == 0)
        {
            slogd("Setting simulator to Test Data");
            rs->sim_name = SIMULATOR_SIMAPI_TEST;
        }
        else
        {
            slogi("%s does not appear to be a supported simulator.", game);
            return RACEENGINEER_ERROR_INVALID_SIM;
        }
    return RACEENGINEER_ERROR_NONE;
}

int set_basic(RaceEngineerSettings* rs, const config_setting_t* config_metric, Metric* m)
{
    slogt("setting basic fields");
    m->maxind = false;
    m->lastplaylap = -2;
    m->laststate = -1;
    m->lastplaytime = 0;

    const char* temp;
    config_setting_lookup_string(config_metric, CONFIGSTR_REPEAT, &temp);
    if (rs->program_action == A_SINGLESHOT)
    {
        m->repeat = ONCE;
    }
    else
    {
        if (strcasecmp(temp, "always") == 0)
        {
            m->repeat = ALWAYS;
        }
        else if (strcasecmp(temp, "once") == 0)
        {
            m->repeat = ONCE;
        }
        else if (strcasecmp(temp, "lap") == 0)
        {
            m->repeat = LAP;
        }
        else
        {
            m->repeat = ONCHANGE;
        }
    }

    int found = config_setting_lookup_string(config_metric, "afile0", &temp);
    if (found == 0)
    {
        m->afile0 = NULL;
    }
    else
    {
        char* temp4 = malloc(1 + strlen(temp) + strlen(rs->sounds_path));
        strcpy(temp4, rs->sounds_path);
        strcat(temp4, temp);
        m->afile0 = strdup(temp4);
        free(temp4);
    }

    config_setting_lookup_string(config_metric, "name", &temp);
    m->name = strdup(temp);
    found = config_setting_lookup_string(config_metric, "variable", &temp);
    if (found == 0)
    {
        m->variable = NULL;
    }
    else
    {
        m->variable = strdup(temp);
    }
    config_setting_lookup_bool(config_metric, "enabled", &m->enabled);

    config_setting_lookup_string(config_metric, CONFIGSTR_MINMAX, &temp);
    if (strcmp(temp, "max") == 0)
    {
        m->maxind = true;
    }

    config_setting_t* config_afiles = NULL;
    const char* path = "afiles";
    config_afiles = config_setting_get_member(config_metric, path);
    m->afilecount = 0;
    if (config_afiles != NULL)
    {
        int clen = config_setting_length(config_afiles);
        m->afilecount = clen;
        for (int i=0; i<clen; i++)
        {
            config_setting_t* afile;
            char* temp2 = strdup(config_setting_get_string_elem(config_afiles, i));
            char* temp3 = malloc(1 + strlen(temp2) + strlen(rs->sounds_path));
            strcpy(temp3, rs->sounds_path);
            strcat(temp3, temp2);
            m->afiles[i] = strdup(temp3);
            free(temp2);
            free(temp3);
        }
    }
    return 0;
}

int loadconfig_scan(config_t* cfg)
{

    slogi("initial scan of raceengineer configuration file");
    config_setting_t* config_metrics = NULL;
    const char* path = "monitor";
    config_metrics = config_lookup(cfg, path);
    int clen = config_setting_length(config_metrics);

    slogi("settings length is %i", clen);
    config_setting_t* config_metric = NULL;

    for (int j = 0; j < clen; j++)
    {
        const char* tname;
        config_metric = config_setting_get_elem(config_metrics, j);
        config_setting_lookup_string(config_metric, "name", &tname);
    }

    return clen;
}

int loadconfig(RaceEngineerSettings* rs, config_t* cfg, Metric* metrics, int nummetrics, struct Map* map, int mapsize)
{

    slogi("creating dictionary of report metrics from configuration file for the race engineer");
    config_setting_t* config_metrics = NULL;
    config_metrics = config_lookup(cfg, "monitor");
    int clen = config_setting_length(config_metrics);
    slogd("will attempt to read %i settings", clen);
    config_setting_t* config_metric = NULL;


    const char* reporttype;
    const char* datatype = NULL;
    for (int j = 0; j < clen; j++)
    {
        config_metric = config_setting_get_elem(config_metrics, j);
        datatype = NULL;
        config_setting_lookup_string(config_metric, CONFIGSTR_REPORTTYPE, &reporttype);
        int found = config_setting_lookup_string(config_metric, CONFIGSTR_DATATYPE, &datatype);

        if (found == 0)
        {
            const char* temp;
            config_setting_lookup_string(config_metric, "variable", &temp);
            for (int k = 0; k < mapsize; k++)
            {
                if (strcmp(map[k].name, temp) == 0)
                {
                    switch ( map[k].dtype )
                    {
                        case INTEGER:
                            datatype = CONFIGVAL_INTEGER;
                            break;
                        case FLOAT:
                            datatype = CONFIGVAL_FLOAT;
                            break;
                        case DOUBLE:
                            datatype = CONFIGVAL_DOUBLE;
                            break;
                    }
                }
            }
        }

        if (strcmp(reporttype, CONFIGVAL_BASICREPORT) == 0)
        {
            slogt("loading basic report metric");
            if (strcmp(datatype, CONFIGVAL_FLOAT) == 0)
            {
                FloatMetric* fm = new_float_metric();
                set_basic(rs, config_metric, &fm->m);
                slogt("creating basic float metric with name %s", fm->m.name);

                config_setting_t* config_thresh = NULL;
                const char* path = "thresh";
                config_thresh = config_setting_get_member(config_metric, path);
                if (config_thresh != NULL)
                {
                    int len = config_setting_length(config_thresh);
                    for (int k=0; k<len; k++)
                    {
                        double d;
                        d = config_setting_get_float_elem(config_thresh, k);
                        fm->thresh[k] = (float) d;
                    }
                }
                metrics[j] = fm->m;
            }
            else if (strcmp(datatype, CONFIGVAL_DOUBLE) == 0)
            {
                DoubleMetric* dm = new_double_metric();
                set_basic(rs, config_metric, &dm->m);
                slogt("creating basic double metric with name %s", dm->m.name);

                config_setting_t* config_thresh = NULL;
                const char* path = "thresh";
                config_thresh = config_setting_get_member(config_metric, path);
                if (config_thresh != NULL)
                {
                    int len = config_setting_length(config_thresh);
                    for (int k=0; k<len; k++)
                    {
                        double d;
                        d = config_setting_get_float_elem(config_thresh, k);
                        dm->thresh[k] = d;
                    }
                }

                metrics[j] = dm->m;
            }
            else if (strcmp(datatype, CONFIGVAL_INTEGER) == 0)
            {
                IntegerMetric* im = new_integer_metric();
                set_basic(rs, config_metric, &im->m);

                config_setting_t* config_thresh = NULL;
                const char* path = "thresh";
                config_thresh = config_setting_get_member(config_metric, path);
                if (config_thresh != NULL)
                {
                    int len = config_setting_length(config_thresh);
                    for (int k=0; k<len; k++)
                    {
                        double d;
                        d = config_setting_get_float_elem(config_thresh, k);
                        im->thresh[k] = d;
                    }
                }

                metrics[j] = im->m;
            }
            else
            {
                slogi("Invalid data type for metric: \"%s\", ignoring", datatype);
            }

        }
    }

    slogd("loading dependent metrics");
    // non basic types
    for (int j = 0; j < clen; j++)
    {
        config_metric = config_setting_get_elem(config_metrics, j);
        config_setting_lookup_string(config_metric, CONFIGSTR_REPORTTYPE, &reporttype);
        int found = config_setting_lookup_string(config_metric, CONFIGSTR_DATATYPE, &datatype);

        if (found == 0)
        {
            const char* temp;
            config_setting_lookup_string(config_metric, "variable", &temp);
            for (int k = 0; k < mapsize; k++)
            {
                if (strcmp(map[k].name, temp) == 0)
                {
                    switch ( map[k].dtype )
                    {
                        case INTEGER:
                            datatype = CONFIGVAL_INTEGER;
                            break;
                        case FLOAT:
                            datatype = CONFIGVAL_FLOAT;
                            break;
                        case DOUBLE:
                            datatype = CONFIGVAL_DOUBLE;
                            break;
                    }
                }
            }
        }

        if (strcmp(reporttype, CONFIGVAL_BASICREPORT) != 0)
        {
            if (strcmp(reporttype, CONFIGVAL_SUMREPORT) == 0 && strcmp(datatype, CONFIGVAL_DOUBLE) == 0)
            {
                SummaryDoubleMetric* sdm = new_summary_double_metric();

                set_basic(rs, config_metric, &sdm->m);


                const char* m1name;
                const char* m2name;
                const char* m3name;
                const char* m4name;

                config_setting_lookup_string(config_metric, "ref1", &m1name);
                config_setting_lookup_string(config_metric, "ref2", &m2name);
                config_setting_lookup_string(config_metric, "ref3", &m3name);
                config_setting_lookup_string(config_metric, "ref4", &m4name);

                int links = 0;
                for (int k = 0; k < j; k++)
                {
                    if (strcmp(metrics[k].name, m1name) == 0)
                    {
                        links++;
                        sdm->m.metric1 = &metrics[k];
                    }
                    if (strcmp(metrics[k].name, m2name) == 0)
                    {
                        links++;
                        sdm->m.metric2 = &metrics[k];
                    }
                    if (strcmp(metrics[k].name, m3name) == 0)
                    {
                        links++;
                        sdm->m.metric3 = &metrics[k];
                    }
                    if (strcmp(metrics[k].name, m4name) == 0)
                    {
                        links++;
                        sdm->m.metric4 = &metrics[k];
                    }
                }
                slogd("found %i links for metric %s", links, sdm->m.name);

                metrics[j] = sdm->m;
            }

            if (strcmp(reporttype, CONFIGVAL_SUMREPORT) == 0 && strcmp(datatype, CONFIGVAL_FLOAT) == 0)
            {
                SummaryFloatMetric* sfm = new_summary_float_metric();

                set_basic(rs, config_metric, &sfm->m);


                const char* m1name;
                const char* m2name;
                const char* m3name;
                const char* m4name;

                config_setting_lookup_string(config_metric, "ref1", &m1name);
                config_setting_lookup_string(config_metric, "ref2", &m2name);
                config_setting_lookup_string(config_metric, "ref3", &m3name);
                config_setting_lookup_string(config_metric, "ref4", &m4name);

                int links = 0;
                for (int k = 0; k < j; k++)
                {
                    if (strcmp(metrics[k].name, m1name) == 0)
                    {
                        links++;
                        sfm->m.metric1 = &metrics[k];
                    }
                    if (strcmp(metrics[k].name, m2name) == 0)
                    {
                        links++;
                        sfm->m.metric2 = &metrics[k];
                    }
                    if (strcmp(metrics[k].name, m3name) == 0)
                    {
                        links++;
                        sfm->m.metric3 = &metrics[k];
                    }
                    if (strcmp(metrics[k].name, m4name) == 0)
                    {
                        links++;
                        sfm->m.metric4 = &metrics[k];
                    }
                }
                slogd("found %i links for metric %s", links, sfm->m.name);

                sfm->m.derived = NULL;
                metrics[j] = sfm->m;
            }

            if (strcmp(reporttype, CONFIGVAL_SUMREPORT) == 0 && strcmp(datatype, CONFIGVAL_INTEGER) == 0)
            {
                SummaryIntegerMetric* sim = new_summary_integer_metric();

                set_basic(rs, config_metric, &sim->m);


                const char* m1name;
                const char* m2name;
                const char* m3name;
                const char* m4name;

                config_setting_lookup_string(config_metric, "ref1", &m1name);
                config_setting_lookup_string(config_metric, "ref2", &m2name);
                config_setting_lookup_string(config_metric, "ref3", &m3name);
                config_setting_lookup_string(config_metric, "ref4", &m4name);

                int links = 0;
                for (int k = 0; k < j; k++)
                {
                    if (strcmp(metrics[k].name, m1name) == 0)
                    {
                        links++;
                        sim->m.metric1 = &metrics[k];
                    }
                    if (strcmp(metrics[k].name, m2name) == 0)
                    {
                        links++;
                        sim->m.metric2 = &metrics[k];
                    }
                    if (strcmp(metrics[k].name, m3name) == 0)
                    {
                        links++;
                        sim->m.metric3 = &metrics[k];
                    }
                    if (strcmp(metrics[k].name, m4name) == 0)
                    {
                        links++;
                        sim->m.metric4 = &metrics[k];
                    }
                }
                slogd("found %i links for metric %s", links, sim->m.name);

                sim->m.derived = NULL;
                metrics[j] = sim->m;
            }
        }
    }

    return RACEENGINEER_ERROR_NONE;
}
