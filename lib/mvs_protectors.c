#include <mvs_protectors.h>

mResult_t mvs_mutex_init(mmutex_t *lock) {
  if (!lock)
    return MRES_INVALID_ARGS;
#if defined(_USE_LINUX_)
  if (pthread_mutex_init(lock, NULL) != 0)
    return MRES_SYS_FAILURE;

#else
// ...
#endif
  return MRES_SUCCESS;
}

mResult_t mvs_cond_init(mcond_t *cond) {
  if (!cond)
    return MRES_INVALID_ARGS;
#if defined(_USE_LINUX_)
  if (pthread_cond_init(cond, NULL) != 0)
    return MRES_SYS_FAILURE;
#else
// ...
#endif
  return MRES_SUCCESS;
}

mResult_t mvs_mutex_destroy(mmutex_t *mutex) {
  if (!mutex)
    return MRES_INVALID_ARGS;
#if defined(_USE_LINUX_)
  pthread_mutex_destroy(mutex);
#else
//....
#endif
  return MRES_SUCCESS;
}

mResult_t mvs_cond_destroy(mcond_t *cond) {
  if (!cond)
    return MRES_INVALID_ARGS;
#if defined(_USE_LINUX_)
  pthread_cond_destroy(cond);
#endif
  return MRES_SUCCESS;
}

mResult_t mvs_mutex_lock(mmutex_t *mutex) {
  if (!mutex)
    return MRES_INVALID_ARGS;
#if defined(_USE_LINUX_)
  pthread_mutex_lock(mutex);
#else
#endif
  return MRES_SUCCESS;
}

mResult_t mvs_mutex_unlock(mmutex_t *mutex) {
  if (!mutex)
    return MRES_INVALID_ARGS;
#if defined(_USE_LINUX_)
  pthread_mutex_unlock(mutex);
#else
#endif
  return MRES_SUCCESS;
}

mResult_t mvs_cond_wait(mcond_t *cond, mmutex_t *lock) {
  if (!lock || !cond)
    return MRES_INVALID_ARGS;
#if defined(_USE_LINUX_)
  pthread_cond_wait(cond, lock);
#else
#endif
  return MRES_SUCCESS;
}

mResult_t mvs_cond_signal(mcond_t *cond) {
  if (!cond)
    return MRES_INVALID_ARGS;
#if defined(_USE_LINUX_)
  pthread_cond_signal(cond);
#elif defined(_USE_WIN_)
  WakeConditionVariable(cond);
#endif
  return MRES_SUCCESS;
}

mResult_t mvs_cond_broadcast(mcond_t *cond) {
  if (!cond)
    return MRES_INVALID_ARGS;
#if defined(_USE_LINUX_)
  pthread_cond_broadcast(cond);
#else
#endif
  return MRES_SUCCESS;
}
