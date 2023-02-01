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

void resettingsfree(RaceEngineerSettings* rs)
{
    free(rs->metric_name);
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
    const char* temp;
    config_setting_lookup_string(config_metric, "afile0", &temp);
    m->afile0 = strdup(temp);
    config_setting_lookup_string(config_metric, "name", &temp);
    m->name = strdup(temp);
    config_setting_lookup_string(config_metric, "variable", &temp);
    m->variable = strdup(temp);
    config_setting_lookup_bool(config_metric, "enabled", &m->enabled);

    config_setting_lookup_string(config_metric, "minormax", &temp);
    if (strcmp(temp, "max") == 0)
    {
        m->maxind = true;
    }

    config_setting_t* config_afiles = NULL;
    const char* path = "afiles";
    config_afiles = config_setting_get_member(config_metric, path);
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
            m->afiles[i] = temp3;
            //m->afiles[i] = strdup(config_setting_get_string_elem(config_afiles, i));
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

int loadconfig(RaceEngineerSettings* rs, config_t* cfg, Metric* metrics[], int nummetrics)
{

    slogi("creating dictionary of report metrics from configuration file for the race engineer");
    config_setting_t* config_metrics = NULL;
    config_metrics = config_lookup(cfg, "monitor");
    int clen = config_setting_length(config_metrics);
    slogd("will attempt to read %i settings", clen);
    config_setting_t* config_metric = NULL;


    const char* reporttype;
    const char* datatype;
    for (int j = 0; j < clen; j++)
    {
        config_metric = config_setting_get_elem(config_metrics, j);
        config_setting_lookup_string(config_metric, CONFIGSTR_REPORTTYPE, &reporttype);
        config_setting_lookup_string(config_metric, CONFIGSTR_DATATYPE, &datatype);

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

                metrics[j] = &fm->m;
            }
            else if (strcmp(datatype, CONFIGVAL_DOUBLE) == 0)
            {
                DoubleMetric* dm = new_double_metric();
                set_basic(rs, config_metric, &dm->m);

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

                metrics[j] = &dm->m;
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

                metrics[j] = &im->m;
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
        config_setting_lookup_string(config_metric, CONFIGSTR_DATATYPE, &datatype);

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
                    if (strcmp(metrics[k]->name, m1name) == 0)
                    {
                        links++;
                        sdm->m.metric1 = metrics[k];
                    }
                    if (strcmp(metrics[k]->name, m2name) == 0)
                    {
                        links++;
                        sdm->m.metric2 = metrics[k];
                    }
                    if (strcmp(metrics[k]->name, m3name) == 0)
                    {
                        links++;
                        sdm->m.metric3 = metrics[k];
                    }
                    if (strcmp(metrics[k]->name, m4name) == 0)
                    {
                        links++;
                        sdm->m.metric4 = metrics[k];
                    }
                }
                slogd("found %i links for metric %s", links, sdm->m.name);

                metrics[j] = &sdm->m;
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
                    if (strcmp(metrics[k]->name, m1name) == 0)
                    {
                        links++;
                        sfm->m.metric1 = metrics[k];
                    }
                    if (strcmp(metrics[k]->name, m2name) == 0)
                    {
                        links++;
                        sfm->m.metric2 = metrics[k];
                    }
                    if (strcmp(metrics[k]->name, m3name) == 0)
                    {
                        links++;
                        sfm->m.metric3 = metrics[k];
                    }
                    if (strcmp(metrics[k]->name, m4name) == 0)
                    {
                        links++;
                        sfm->m.metric4 = metrics[k];
                    }
                }
                slogd("found %i links for metric %s", links, sfm->m.name);

                metrics[j] = &sfm->m;
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
                    if (strcmp(metrics[k]->name, m1name) == 0)
                    {
                        links++;
                        sim->m.metric1 = metrics[k];
                    }
                    if (strcmp(metrics[k]->name, m2name) == 0)
                    {
                        links++;
                        sim->m.metric2 = metrics[k];
                    }
                    if (strcmp(metrics[k]->name, m3name) == 0)
                    {
                        links++;
                        sim->m.metric3 = metrics[k];
                    }
                    if (strcmp(metrics[k]->name, m4name) == 0)
                    {
                        links++;
                        sim->m.metric4 = metrics[k];
                    }
                }
                slogd("found %i links for metric %s", links, sim->m.name);

                metrics[j] = &sim->m;
            }
        }
    }

    return RACEENGINEER_ERROR_NONE;
}

