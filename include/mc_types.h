#ifndef MC_TYPES_H
#define MC_TYPES_H

#include <stdint.h>
#include <stddef.h>
#include "mc_config.h"

#ifdef MC_USE_FLOAT
    typedef float mc_real_t;
    #define MC_FP_SCALE  1.0f
    #define MC_FP_C(v)   (v)
#else
    typedef int32_t mc_real_t;
    #define MC_FP_SCALE  65536
    #define MC_FP_C(v)   ((mc_real_t)((v) * MC_FP_SCALE))
#endif

#define MC_REAL_FROM_INT(n)      ((mc_real_t)(n) * MC_FP_SCALE)
#define MC_REAL_FROM_FLOAT(f)    ((mc_real_t)((f) * MC_FP_SCALE))
#define MC_REAL_FROM_DOUBLE(d)   ((mc_real_t)((d) * MC_FP_SCALE))
#define MC_REAL_TO_FLOAT(r)      ((float)(r) / MC_FP_SCALE)
#define MC_REAL_TO_DOUBLE(r)     ((double)(r) / MC_FP_SCALE)
#define MC_REAL_TO_INT(r)        ((int)((r) / MC_FP_SCALE))

#endif
