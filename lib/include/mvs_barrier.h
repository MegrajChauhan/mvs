#ifndef _MVS_BARRIER_
#define _MVS_BARRIER_

#include <mvs_platform.h>
#include <mvs_results.h>
#include <mvs_types.h>

typedef struct MVSBarrier MVSBarrier;

/*
 * This barrier is shared through references and doesn't require malloc'ing
 * Currently, lib is only abstracting the platform provided primitives. If the
 * demand ever came for custom implementation, then lib will implement its own
 * barrier using futex(for linux) and whatever exists for windows and other
 * platforms.
 * */
struct MVSBarrier {
  mbarrier_t barrier;
  mbool_t init;
};

mResult_t mvs_barrier_init(MVSBarrier *barrier);

mResult_t mvs_barrier_wait(MVSBarrier *barrier);

mResult_t mvs_barrier_signal(MVSBarrier *barrier);

mResult_t mvs_barrier_destroy(MVSBarrier *barrier);

#endif
