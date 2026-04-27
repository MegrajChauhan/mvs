#ifndef _MVS_THREADS_
#define _MVS_THREADS_

#include <mvs_config.h>
#include <mvs_platform.h>
#include <mvs_results.h>
#include <mvs_types.h>

mResult_t mvs_thread_create(mthread_t *th, mthexec_t func, mptr_t arg);

mResult_t mvs_create_detached_thread(mthread_t *th, mthexec_t func, mptr_t arg);

mResult_t mvs_thread_join(mthread_t thread, mptr_t return_val);

#endif
