#ifndef _METRIC_H
#define _METRIC_H

#include <stdbool.h>
#include <stdint.h>

typedef enum MetricType { SINGLEFLOAT, SINGLEINTEGER, SINGLEDOUBLE, SUMFLOAT, SUMINTEGER, SUMDOUBLE } MetricType;

typedef enum RepeatFreq { ONCE, LAP, ALWAYS, ONCHANGE } RepeatFreq;

typedef struct Metric Metric;

struct Metric
{
    //const struct MetricVtable* const vtable;
    const void* vtable;
    void (*eval)(Metric*, int);
    void (*freemetric) (Metric*);
    void* derived;
    MetricType type;
    RepeatFreq repeat;
    const char* name;
    const char* variable;
    int enabled;
    bool maxind;
    int lastplaylap;
    int laststate;
    uint64_t lastplaytime;
    const char* afile1;
    const char* afile2;
    const char* afile0;
    const char* afiles[10];
    int afilecount;
    void* metric1;
    void* metric2;
    void* metric3;
    void* metric4;
};

typedef struct {
    int (*eval)(Metric*, int);
    void (*freemetric)(Metric*);
} vtable;


typedef struct
{
    Metric m;
    int thresh1;
    int thresh2;
    int thresh[10];
    int* value;
}
IntegerMetric;

typedef struct
{
    Metric m;
    double thresh[10];
    double thresh1;
    double thresh2;
    double* value;
}
DoubleMetric;

typedef struct
{
    Metric m;
    float thresh[10];
    float thresh1;
    float thresh2;
    float* value;
}
FloatMetric;

typedef struct
{
    Metric m;
}
SummaryIntegerMetric;

typedef struct
{
    Metric m;
}
SummaryDoubleMetric;

typedef struct
{
    Metric m;
}
SummaryFloatMetric;


// integers
int integer_metric_eval (Metric* m, int lap);
int summary_integer_metric_eval (Metric* m, int lap);
IntegerMetric* new_integer_metric();
SummaryIntegerMetric* new_summary_integer_metric();
void free_integer_metric(Metric* this);
void free_summary_integer_metric(Metric* this);

// doubles
int double_metric_eval (Metric* m, int lap);
int summary_double_metric_eval (Metric* m, int lap);
DoubleMetric* new_double_metric();
SummaryDoubleMetric* new_summary_double_metric();
void free_double_metric(Metric* this);
void free_summary_double_metric(Metric* this);

// floats
int float_metric_eval (Metric* m, int lap);
int summary_float_metric_eval (Metric* m, int lap);
FloatMetric* new_float_metric();
SummaryFloatMetric* new_summary_float_metric();
void free_float_metric(Metric* this);
void free_summary_float_metric(Metric* this);

#endif
