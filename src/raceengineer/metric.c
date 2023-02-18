#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>


#include "metric.h"
#include "playwav.h"

#include "slog/slog.h"

int doplay(RepeatFreq repeat, uint64_t* lastplaytime, int* lastplaylap, int lap, int* lastthresh, int thresh, const char* afile, bool singleshot)
{
    if(strcmp(afile, "") == 0)
    {
        return 1;
    }
    if(repeat == LAP && lap > (*lastplaylap))
    {
        play(afile);
        *lastplaylap = lap;
        *lastthresh = thresh;
        *lastplaytime = 1;
    }
    if((repeat == ONCHANGE || repeat == ONCE) && thresh != (*lastthresh))
    {
        play(afile);
        if (repeat == ONCE)
        {
            return 0;
        }
        *lastthresh = thresh;
        *lastplaylap = lap;
        *lastplaytime = 1;
    }
    return 0;
}

void metricfree(Metric* this)
{
    ((vtable*)this->vtable)->freemetric(this);
}

void metriceval(Metric* this, int lap)
{
    ((vtable*)this->vtable)->eval(this, lap);
}

// integers
int integer_metric_eval (Metric* m, int lap)
{
    IntegerMetric* im = (void *) m->derived;
    int i = *(int*) m->value;

    bool played = false;
    if (m->maxind == false)
    {
        for(int j=0; j<m->afilecount; j++)
        {
            if (i < im->thresh[j])
            {
                doplay(m->repeat, &m->lastplaytime, &m->lastplaylap, lap, &m->laststate, j, m->afiles[j], false);
                played = true;
                break;
            }
        }
    }
    else
    {
        for(int j=0; j<m->afilecount; j++)
        {
            if (i > im->thresh[j])
            {
                doplay(m->repeat, &m->lastplaytime, &m->lastplaylap, lap, &m->laststate, j, m->afiles[j], false);
                played = true;
                break;
            }
        }
    }

    if (played == false && lap < 0)
    {
        doplay(ONCE, &m->lastplaytime, &m->lastplaylap, lap, &m->laststate, 0, m->afile0, true);
    }

    return 0;
}

int summary_integer_metric_eval (Metric* m, int lap)
{
    Metric* imm1 = (void *) m->metric1;
    IntegerMetric* im1 = (void *) imm1->derived;
    int i1 = *(int*) imm1->value;

    Metric* imm2 = (void *) m->metric2;
    IntegerMetric* im2 = (void *) imm2->derived;
    int i2 = *(int*) imm2->value;

    Metric* imm3 = (void *) m->metric3;
    IntegerMetric* im3 = (void *) imm3->derived;
    int i3 = *(int*) imm3->value;

    Metric* imm4 = (void *) m->metric4;
    IntegerMetric* im4 = (void *) imm4->derived;
    int i4 = *(int*) imm4->value;

    bool played = false;
    if (m->maxind == false)
    {
        for(int j=0; j<m->afilecount; j++)
        {
            if (i1 < im1->thresh[j] && i2 < im2->thresh[j] && i3 < im3->thresh[j] && i4 < im4->thresh[j])
            {
                doplay(m->repeat, &m->lastplaytime, &m->lastplaylap, lap, &m->laststate, j, m->afiles[j], false);
                played = true;
                break;
            }
        }
    }
    else
    {
        for(int j=0; j<m->afilecount; j++)
        {
            if (i1 > im1->thresh[j] && i2 > im2->thresh[j] && i3 > im3->thresh[j] && i4 > im4->thresh[j])
            {
                doplay(m->repeat, &m->lastplaytime, &m->lastplaylap, lap, &m->laststate, j, m->afiles[j], false);
                played = true;
                break;
            }
        }
    }

    if (played == false && lap < 0)
    {
        doplay(ONCE, &m->lastplaytime, &m->lastplaylap, lap, &m->laststate, 0, m->afile0, true);
    }
    return 0;
}

// doubles
int double_metric_eval (Metric* m, int lap)
{
    DoubleMetric* dm = (void *) m->derived;
    double i = *(double*) m->value;

    bool played = false;
    if (m->maxind == false)
    {
        for(int j=0; j<m->afilecount; j++)
        {
            if (i < dm->thresh[j])
            {
                doplay(m->repeat, &m->lastplaytime, &m->lastplaylap, lap, &m->laststate, j, m->afiles[j], false);
                played = true;
                break;
            }
        }
    }
    else
    {
        for(int j=0; j<m->afilecount; j++)
        {
            if (i > dm->thresh[j])
            {
                doplay(m->repeat, &m->lastplaytime, &m->lastplaylap, lap, &m->laststate, j, m->afiles[j], false);
                played = true;
                break;
            }
        }
    }

    if (played == false && lap < 0)
    {
        doplay(ONCE, &m->lastplaytime, &m->lastplaylap, lap, &m->laststate, 0, m->afile0, true);
    }

    return 0;
}

int summary_double_metric_eval (Metric* m, int lap)
{
    Metric* dmm1 = (void *) m->metric1;
    DoubleMetric* dm1 = (void *) dmm1->derived;
    double d1 = *(double*) dmm1->value;

    Metric* dmm2 = (void *) m->metric2;
    DoubleMetric* dm2 = (void *) dmm2->derived;
    double d2 = *(double*) dmm2->value;

    Metric* dmm3 = (void *) m->metric3;
    DoubleMetric* dm3 = (void *) dmm3->derived;
    double d3 = *(double*) dmm3->value;

    Metric* dmm4 = (void *) m->metric4;
    DoubleMetric* dm4 = (void *) dmm4->derived;
    double d4 = *(double*) dmm4->value;

    bool played = false;
    if (m->maxind == false)
    {
        for(int j=0; j<m->afilecount; j++)
        {
            if (d1 < dm1->thresh[j] && d2 < dm2->thresh[j] && d3 < dm3->thresh[j] && d4 < dm4->thresh[j])
            {
                doplay(m->repeat, &m->lastplaytime, &m->lastplaylap, lap, &m->laststate, j, m->afiles[j], false);
                played = true;
                break;
            }
        }
    }
    else
    {
        for(int j=0; j<m->afilecount; j++)
        {
            if (d1 > dm1->thresh[j] && d2 > dm2->thresh[j] && d3 > dm3->thresh[j] && d4 > dm4->thresh[j])
            {
                doplay(m->repeat, &m->lastplaytime, &m->lastplaylap, lap, &m->laststate, j, m->afiles[j], false);
                played = true;
                break;
            }
        }
    }

    if (played == false && lap < 0)
    {
        doplay(ONCE, &m->lastplaytime, &m->lastplaylap, lap, &m->laststate, 0, m->afile0, true);
    }
    return 0;
}

// floats
int float_metric_eval (Metric* m, int lap)
{
    FloatMetric* fm = (void *) m->derived;
    float f = *(float*) m->value;

    bool played = false;
    if (m->maxind == false)
    {
        for(int j=0; j<m->afilecount; j++)
        {
            if (f < fm->thresh[j])
            {
                doplay(m->repeat, &m->lastplaytime, &m->lastplaylap, lap, &m->laststate, j, m->afiles[j], false);
                played = true;
                break;
            }
        }
    }
    else
    {
        for(int j=0; j<m->afilecount; j++)
        {
            if (f > fm->thresh[j])
            {
                doplay(m->repeat, &m->lastplaytime, &m->lastplaylap, lap, &m->laststate, j, m->afiles[j], false);
                played = true;
                break;
            }
        }
    }

    if (played == false && lap < 0)
    {
        doplay(ONCE, &m->lastplaytime, &m->lastplaylap, lap, &m->laststate, 0, m->afile0, true);
        slogt("Metric value is %f", f);
    }

    return 0;
}


int summary_float_metric_eval (Metric* m, int lap)
{
    Metric* fmm1 = (void *) m->metric1;
    FloatMetric* fm1 = (void *) fmm1->derived;
    float f1 = *(float*) fmm1->value;

    Metric* fmm2 = (void *) m->metric2;
    FloatMetric* fm2 = (void *) fmm2->derived;
    float f2 = *(float*) fmm2->value;

    Metric* fmm3 = (void *) m->metric3;
    FloatMetric* fm3 = (void *) fmm3->derived;
    float f3 = *(float*) fmm3->value;

    Metric* fmm4 = (void *) m->metric4;
    FloatMetric* fm4 = (void *) fmm4->derived;
    float f4 = *(float*) fmm4->value;

    bool played = false;
    if (m->maxind == false)
    {
        for(int j=0; j<m->afilecount; j++)
        {
            if (f1 < fm1->thresh[j] && f2 < fm2->thresh[j] && f3 < fm3->thresh[j] && f4 < fm4->thresh[j])
            {
                doplay(m->repeat, &m->lastplaytime, &m->lastplaylap, lap, &m->laststate, j, m->afiles[j], false);
                played = true;
                break;
            }
        }
    }
    else
    {
        for(int j=0; j<m->afilecount; j++)
        {
            if (f1 > fm1->thresh[j] && f2 > fm2->thresh[j] && f3 > fm3->thresh[j] && f4 > fm4->thresh[j])
            {
                doplay(m->repeat, &m->lastplaytime, &m->lastplaylap, lap, &m->laststate, j, m->afiles[j], false);
                played = true;
                break;
            }
        }
    }

    if (played == false && lap < 0)
    {
        doplay(ONCE, &m->lastplaytime, &m->lastplaylap, lap, &m->laststate, 0, m->afile0, true);
        slogt("Metric values are %f, %f, %f, %f", f1, f2, f3, f4);
    }
    return 0;
}

void free_float_metric(Metric* this)
{
    FloatMetric* fm = (void *) this->derived;
    free(fm);
}
static const vtable float_metric_vtable = { &float_metric_eval, &free_float_metric };

FloatMetric* new_float_metric()
{
    FloatMetric* this = (FloatMetric*) malloc(sizeof(FloatMetric));
    this->m.eval = &metriceval;
    this->m.derived = this;
    this->m.type = SINGLEFLOAT;
    this->m.vtable = &float_metric_vtable;
    return this;
}

void free_double_metric(Metric* this)
{
    DoubleMetric* dm = (void *) this->derived;
    free(dm);
}
static const vtable double_metric_vtable = { &double_metric_eval, &free_double_metric };

DoubleMetric* new_double_metric()
{
    DoubleMetric* this = (DoubleMetric*) malloc(sizeof(DoubleMetric));
    this->m.eval = &metriceval;
    this->m.derived = this;
    this->m.type = SINGLEDOUBLE;
    this->m.vtable = &double_metric_vtable;
    return this;
}

void free_integer_metric(Metric* this)
{
    IntegerMetric* im = (void *) this->derived;
    free(im);
}
static const vtable integer_metric_vtable = { &integer_metric_eval, &free_integer_metric };

IntegerMetric* new_integer_metric()
{
    IntegerMetric* this = (IntegerMetric*) malloc(sizeof(IntegerMetric));
    this->m.eval = &metriceval;
    this->m.derived = this;
    this->m.type = SINGLEINTEGER;
    this->m.vtable = &integer_metric_vtable;
    return this;
}

void free_summary_float_metric(Metric* this)
{
    SummaryFloatMetric* sfm = (void *) this->derived;
    free(sfm);
}
static const vtable summary_float_metric_vtable = { &summary_float_metric_eval, &free_summary_float_metric };

SummaryFloatMetric* new_summary_float_metric()
{
    SummaryFloatMetric* this = (SummaryFloatMetric*) malloc(sizeof(SummaryFloatMetric));
    this->m.eval = &metriceval;
    this->m.derived = this;
    this->m.type = SUMFLOAT;
    this->m.vtable = &summary_float_metric_vtable;
    return this;
}

void free_summary_double_metric(Metric* this)
{
    SummaryDoubleMetric* sdm = (void *) this->derived;
    free(sdm);
}
static const vtable summary_double_metric_vtable = { &summary_double_metric_eval, &free_summary_double_metric };

SummaryDoubleMetric* new_summary_double_metric()
{
    SummaryDoubleMetric* this = (SummaryDoubleMetric*) malloc(sizeof(SummaryDoubleMetric));
    this->m.eval = &metriceval;
    this->m.derived = this;
    this->m.type = SUMDOUBLE;
    this->m.vtable = &summary_double_metric_vtable;
    return this;
}

void free_summary_integer_metric(Metric* this)
{
    SummaryIntegerMetric* sim = (void *) this->derived;
    free(sim);
}
static const vtable summary_integer_metric_vtable = { &summary_integer_metric_eval, &free_summary_integer_metric };

SummaryIntegerMetric* new_summary_integer_metric()
{
    SummaryIntegerMetric* this = (SummaryIntegerMetric*) malloc(sizeof(SummaryIntegerMetric));
    this->m.eval = &metriceval;
    this->m.derived = this;
    this->m.type = SUMINTEGER;
    this->m.vtable = &summary_integer_metric_vtable;
    return this;
}



