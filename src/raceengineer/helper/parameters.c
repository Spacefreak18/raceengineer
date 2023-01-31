#include "parameters.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libconfig.h>

#include <argtable2.h>
#include <regex.h>


ConfigError getParameters(int argc, char** argv, Parameters* p)
{
    ConfigError exitcode = E_SOMETHING_BAD;
    // set return structure defaults
    p->program_action      = 0;
    p->verbosity_count     = 0;

    // setup argument handling structures
    const char* progname = "raceengineer";

    struct arg_lit* arg_verbosity1   = arg_litn("v","verbose", 0, 2, "increase logging verbosity");
    struct arg_lit* arg_verbosity2   = arg_litn("v","verbose", 0, 2, "increase logging verbosity");

    struct arg_rex* cmd1             = arg_rex1(NULL, NULL, "monitor", NULL, REG_ICASE, NULL);
    struct arg_str* arg_sim1         = arg_strn("s", "sim", "<gamename>", 1, 1, NULL);
    struct arg_str* arg_cpath1       = arg_strn("c", "configpath", "<path of config>", 0, 1, NULL);
    struct arg_str* arg_spath1       = arg_strn("p", "soundpath", "<path of sound files>", 0, 1, NULL);
    struct arg_lit* help1            = arg_litn(NULL,"help", 0, 1, "print this help and exit");
    struct arg_lit* version1         = arg_litn(NULL,"version", 0, 1, "print version information and exit");
    struct arg_end* end1             = arg_end(20);

    struct arg_rex* cmd2             = arg_rex1(NULL, NULL, "singleshot", NULL, REG_ICASE, NULL);
    struct arg_str* arg_metric       = arg_strn("m", "metric", "<metricname>", 1, 1, NULL);
    struct arg_str* arg_sim2         = arg_strn("s", "sim", "<gamename>", 1, 1, NULL);
    struct arg_str* arg_cpath2       = arg_strn("c", "configpath", "<path of config>", 0, 1, NULL);
    struct arg_str* arg_spath2       = arg_strn("p", "soundpath", "<path of sound files>", 0, 1, NULL);
    struct arg_lit* help2            = arg_litn(NULL,"help", 0, 1, "print this help and exit");
    struct arg_lit* version2         = arg_litn(NULL,"version", 0, 1, "print version information and exit");
    struct arg_end* end2             = arg_end(20);


    struct arg_lit* help             = arg_litn(NULL,"help", 0, 1, "print this help and exit");
    struct arg_lit* version          = arg_litn(NULL,"version", 0, 1, "print version information and exit");
    struct arg_end* end              = arg_end(20);


    void* argtable1[]                 = {cmd1,arg_sim1,arg_cpath1,arg_spath1,arg_verbosity1,help1,version1,end1};
    void* argtable2[]                 = {cmd2,arg_sim2,arg_metric,arg_cpath2,arg_spath2,arg_verbosity2,help2,version2,end2};
    void* argtable0[]                 = {help,version,end};

    int nerrors0;
    int nerrors1;
    int nerrors2;


    if (arg_nullcheck(argtable0) != 0)
    {
        printf("%s: insufficient memory\n",progname);
        goto cleanup;
    }
    if (arg_nullcheck(argtable1) != 0)
    {
        printf("%s: insufficient memory\n",progname);
        goto cleanup;
    }
    if (arg_nullcheck(argtable2) != 0)
    {
        printf("%s: insufficient memory\n",progname);
        goto cleanup;
    }


    nerrors0 = arg_parse(argc,argv,argtable0);
    nerrors1 = arg_parse(argc,argv,argtable1);
    nerrors2 = arg_parse(argc,argv,argtable2);

    if (nerrors1==0)
    {
        p->program_action = A_MONITOR;
        p->sim_string = arg_sim1->sval[0];
        p->verbosity_count = arg_verbosity1->count;
        p->config_path = NULL;
        if (arg_cpath1->sval[0] != NULL)
        {
            p->config_path = arg_cpath1->sval[0];
        }
        p->sounds_path = NULL;
        if (arg_spath1->sval[0] != NULL)
        {
            p->sounds_path = arg_spath1->sval[0];
        }
        exitcode = E_SUCCESS_AND_DO;
        goto cleanup;
    }
    else
    {
        if (nerrors2==0)
        {
            p->sim_string = arg_sim2->sval[0];
            p->program_action = A_SINGLESHOT;
            p->metric_name = arg_metric->sval[0];
            p->verbosity_count = arg_verbosity2->count;
            exitcode = E_SUCCESS_AND_DO;
            goto cleanup;
        }
        else
        {
            if (cmd2->count > 0)
            {
                arg_print_errors(stdout,end2,progname);
                printf("Usage: %s ", progname);
                arg_print_syntax(stdout,argtable2,"\n");
            }
            else
            {
                if (help->count==0 && version->count==0)
                {
                    printf("%s: missing <monitor|singleshot> command.\n",progname);
                    printf("Usage 1: %s ", progname);
                    arg_print_syntax(stdout,argtable1,"\n");
                    printf("Usage 2: %s ", progname);
                    arg_print_syntax(stdout,argtable2,"\n");

                }
            }
        }
    }
    // interpret some special cases before we go through trouble of reading the config file
    if (help->count > 0)
    {
        printf("Usage: %s\n", progname);
        printf("\nReport bugs on the github github.com/spacefreak18/raceengineer.\n");
        exitcode = E_SUCCESS_AND_EXIT;
        goto cleanup;
    }

    if (version->count > 0)
    {
        printf("%s\n",progname);
        printf("February 2023, Paul Dino Jones\n");
        exitcode = E_SUCCESS_AND_EXIT;
        goto cleanup;
    }

cleanup:
    arg_freetable(argtable0,sizeof(argtable0)/sizeof(argtable0[0]));
    arg_freetable(argtable1,sizeof(argtable1)/sizeof(argtable1[0]));
    arg_freetable(argtable2,sizeof(argtable2)/sizeof(argtable2[0]));
    return exitcode;

}
