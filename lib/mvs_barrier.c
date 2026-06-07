#include <mvs_barrier.h>

mResult_t mvs_barrier_init(MVSBarrier *barrier) {
#ifdef _USE_LINUX_
  if (sem_init(&barrier->barrier, 0, 0) != 0)
    return MRES_SYS_FAILURE;
#endif
  barrier->init = mtrue;
  return MRES_SUCCESS;
}

mResult_t mvs_barrier_wait(MVSBarrier *barrier) {
  if (!barrier->init)
    return MRES_RESOURCE_STATE_INVALID;
#ifdef _USE_LINUX_
  if (sem_wait(&barrier->barrier) != 0)
    return MRES_SYS_FAILURE;
#endif
  return MRES_SUCCESS;
}

mResult_t mvs_barrier_signal(MVSBarrier *barrier) {
  if (!barrier->init)
    return MRES_RESOURCE_STATE_INVALID;
#ifdef _USE_LINUX_
  if (sem_post(&barrier->barrier) != 0)
    return MRES_SYS_FAILURE;
#endif
  return MRES_SUCCESS;
}

mResult_t mvs_barrier_destroy(MVSBarrier *barrier) {
  if (!barrier->init)
    return MRES_RESOURCE_STATE_INVALID;
#ifdef _USE_LINUX_
  if (sem_destroy(&barrier->barrier) != 0)
    return MRES_SYS_FAILURE;
#endif
  return MRES_SUCCESS;
}
