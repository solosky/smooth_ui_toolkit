#ifndef SUT_TYPES_H
#define SUT_TYPES_H

#include <stdint.h>
#include <stddef.h>
#include "sut_config.h"

#ifdef SUT_USE_FLOAT
    typedef float sut_real_t;
    #define SUT_FP_SCALE  1.0f
    #define SUT_FP_C(v)   (v)
#else
    typedef int32_t sut_real_t;
    #define SUT_FP_SCALE  65536
    #define SUT_FP_C(v)   ((sut_real_t)((v) * SUT_FP_SCALE))
#endif

#define SUT_REAL_FROM_INT(n)      ((sut_real_t)(n) * SUT_FP_SCALE)
#define SUT_REAL_FROM_FLOAT(f)    ((sut_real_t)((f) * SUT_FP_SCALE))
#define SUT_REAL_FROM_DOUBLE(d)   ((sut_real_t)((d) * SUT_FP_SCALE))
#define SUT_REAL_TO_FLOAT(r)      ((float)(r) / SUT_FP_SCALE)
#define SUT_REAL_TO_DOUBLE(r)     ((double)(r) / SUT_FP_SCALE)
#define SUT_REAL_TO_INT(r)        ((int)((r) / SUT_FP_SCALE))

#endif
