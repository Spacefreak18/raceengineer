#include <stdio.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <libconfig.h>
#include <stdbool.h>
#include <basedir_fs.h>

#include "metric.h"
#include "playwav.h"
#include "gameloop/gameloop.h"
#include "gameloop/singleshot.h"
#include "helper/parameters.h"
#include "helper/dirhelper.h"
#include "helper/confighelper.h"
#include "slog/slog.h"


int create_dir(char* dir)
{
    struct stat st = {0};
    if (stat(dir, &st) == -1)
    {
        mkdir(dir, 0700);
    }
}

char* create_user_dir(const char* dirtype)
{
    char* config_dir_str = ( char* ) malloc(1 + strlen(dirtype) + strlen("/raceengineer"));
    strcpy(config_dir_str, dirtype);
    strcat(config_dir_str, "/raceengineer");

    create_dir(config_dir_str);
    free(config_dir_str);
}

char* get_config_file(const char* confpath, xdgHandle* xdg)
{
    if(strcmp(confpath, "") != 0)
    {
        return strdup(confpath);
    }

    const char* relpath = "raceengineer/raceengineer.config";
    const char* confpath1 = xdgConfigFind(relpath, xdg);
    return strdup(confpath1);
}

char* get_sounds_path(const char* datapath, xdgHandle* xdg)
{
    if(strcmp(datapath, "") != 0)
    {
        return strdup(datapath);
    }

    const char* data_home = xdgDataHome(xdg);
    char* default_sounds = "/raceengineer/sounds/";
    char* fullpath = malloc(1 + strlen(data_home) + strlen(default_sounds));
    strcpy(fullpath, data_home);
    strcat(fullpath, default_sounds);
    slogt("found xdg cache dir %s", data_home);
    return fullpath;
}

void create_cache_path(xdgHandle* xdg)
{
    const char* datapath = xdgCacheHome(xdg);
    create_user_dir(datapath);
}

void display_banner()
{
    printf("________                  __________              _____\n");
    printf("___  __ \\_____ ______________  ____/_____________ ___(_)________________________\n");
    printf("__  /_/ /  __ `/  ___/  _ \\_  __/  __  __ \\_  __ `/_  /__  __ \\  _ \\  _ \\_  ___/\n");
    printf("_  _, _// /_/ // /__ /  __/  /___  _  / / /  /_/ /_  / _  / / /  __/  __/  /\n");
    printf("/_/ |_| \\__,_/ \\___/ \\___//_____/  /_/ /_/_\\__, / /_/  /_/ /_/\\___/\\___//_/\n");
    printf("                                          /____/\n");

}

int main(int argc, char** argv)
{
    display_banner();

    Parameters* p = malloc(sizeof(Parameters));
    RaceEngineerSettings* rs = malloc(sizeof(RaceEngineerSettings));;

    ConfigError ppe = getParameters(argc, argv, p);
    if (ppe == E_SUCCESS_AND_EXIT)
    {
        goto cleanup_final;
    }
    rs->program_action = p->program_action;


    char* home_dir_str = gethome();
    create_user_dir("/.config/");
    create_user_dir("/.cache/");
    char* cache_dir_str = ( char* ) malloc(1 + strlen(home_dir_str) + strlen("/.cache/raceengineer/"));
    strcpy(cache_dir_str, home_dir_str);
    strcat(cache_dir_str, "/.cache/raceengineer/");

    slog_config_t slgCfg;
    slog_config_get(&slgCfg);
    slgCfg.eColorFormat = SLOG_COLORING_TAG;
    slgCfg.eDateControl = SLOG_TIME_ONLY;
    strcpy(slgCfg.sFileName, "raceengineer.log");
    strcpy(slgCfg.sFilePath, cache_dir_str);
    slgCfg.nTraceTid = 0;
    slgCfg.nToScreen = 1;
    slgCfg.nUseHeap = 0;
    slgCfg.nToFile = 1;
    slgCfg.nFlush = 0;
    slgCfg.nFlags = SLOG_FLAGS_ALL;
    slog_config_set(&slgCfg);
    if (p->verbosity_count < 2)
    {
        slog_disable(SLOG_TRACE);
    }
    if (p->verbosity_count < 1)
    {
        slog_disable(SLOG_DEBUG);
    }


    xdgHandle xdg;
    if(!xdgInitHandle(&xdg))
    {
        slogf("Function xdgInitHandle() failed, is $HOME unset?");
    }
    char* config_file_str = get_config_file(p->config_path, &xdg);
    create_cache_path(&xdg);
    rs->sounds_path = get_sounds_path(p->sounds_path, &xdg);
    slogt("using sounds path %s", rs->sounds_path);
    strtogame(p->sim_string, rs);
    slogi("Loading configuration file: %s", config_file_str);
    config_t cfg;
    config_init(&cfg);
    int nummetrics = 0;


    if (!config_read_file(&cfg, config_file_str))
    {
        slogi("%s:%d - %s\n", config_error_file(&cfg), config_error_line(&cfg), config_error_text(&cfg));
    }
    else
    {
        nummetrics = loadconfig_scan(&cfg);
    }

    if (rs->program_action == A_MONITOR)
    {
        slogt("nummetrics is %i", nummetrics);
        Metric* metrics[nummetrics];
        loadconfig(rs, &cfg, metrics, nummetrics);
        config_destroy(&cfg);
        slogt("nummetrics is %i", nummetrics);
        looper(rs, metrics, nummetrics);
    }
    // todo: streamlined loading for singleshot mode
    if (rs->program_action == A_SINGLESHOT)
    {
        rs->metric_name = p->metric_name;
        slogt("nummetrics is %i", nummetrics);
        Metric* metrics[nummetrics];
        loadconfig(rs, &cfg, metrics, nummetrics);
        config_destroy(&cfg);
        slogt("nummetrics is %i", nummetrics);
        singleshot(rs, metrics, nummetrics);
    }

    free(config_file_str);
    free(cache_dir_str);


cleanup_final:
    resettingsfree(rs);
    free(p);
    exit(0);
}



