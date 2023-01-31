#ifndef _PARAMETERS_H
#define _PARAMETERS_H

typedef struct
{
    int   program_action;
    const char* sim_string;
    const char* metric_name;
    const char* sounds_path;
    const char* config_path;
    int verbosity_count;
}
Parameters;

typedef enum
{
    A_MONITOR       = 0,
    A_SINGLESHOT    = 1,
}
ProgramAction;

typedef enum
{
    E_SUCCESS_AND_EXIT = 0,
    E_SUCCESS_AND_DO   = 1,
    E_SOMETHING_BAD    = 2
}
ConfigError;

ConfigError getParameters(int argc, char** argv, Parameters* p);

struct _errordesc
{
    int  code;
    char* message;
} static errordesc[] =
{
    { E_SUCCESS_AND_EXIT, "No error and exiting" },
    { E_SUCCESS_AND_DO,   "No error and continuing" },
    { E_SOMETHING_BAD,    "Something bad happened" },
};

#endif
