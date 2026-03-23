#include <mvs_threads.h>

mResult_t mvs_thread_create(mthread_t *th, mthexec_t func, mptr_t arg) {
  if (!th || !func)
    return MRES_INVALID_ARGS;
#if defined(_USE_LINUX_)
  if (pthread_create(th, NULL, func, arg) != 0) {
    return MRES_SYS_FAILURE;
  }
#else
#endif
  return MRES_SUCCESS;
}

mResult_t mvs_create_detached_thread(mthread_t *th, mthexec_t func,
                                     mptr_t arg) {
  if (!th || !func)
    return MRES_INVALID_ARGS;
#if defined(_USE_LINUX_)
  pthread_attr_t attr;
  if (pthread_attr_init(&attr) != 0) {
    return MRES_SYS_FAILURE; // we failed
  }
  if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0) {
    pthread_attr_destroy(&attr);
    return MRES_SYS_FAILURE;
  }

  if (pthread_create(th, &attr, func, arg) != 0) {
    pthread_attr_destroy(&attr);
    return MRES_SYS_FAILURE;
  }
  pthread_attr_destroy(&attr);
#else
#endif
  return MRES_SUCCESS;
}

mResult_t mvs_thread_join(mthread_t thread, mptr_t return_val) {
#if defined(_USE_LINUX_)
  pthread_join(thread, return_val);
#else
#endif
  return MRES_SUCCESS;
}
