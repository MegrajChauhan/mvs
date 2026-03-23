#ifndef _MVS_PROTECTORS_
#define _MVS_PROTECTORS_

#include <mvs_config.h>
#include <mvs_platform.h>
#include <mvs_results.h>
#include <mvs_tools.h>
#include <mvs_types.h>

mResult_t mvs_mutex_init(mmutex_t *lock);

mResult_t mvs_cond_init(mcond_t *cond);

mResult_t mvs_mutex_destroy(mmutex_t *mutex);

mResult_t mvs_cond_destroy(mcond_t *cond);

mResult_t mvs_mutex_lock(mmutex_t *mutex);

mResult_t mvs_mutex_unlock(mmutex_t *mutex);

mResult_t mvs_cond_wait(mcond_t *cond, mmutex_t *lock);

mResult_t mvs_cond_signal(mcond_t *cond);

mResult_t mvs_cond_broadcast(mcond_t *cond);

#endif
