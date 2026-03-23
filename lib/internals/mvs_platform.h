#ifndef _MVS_PLATFORM_
#define _MVS_PLATFORM_

#include <mvs_config.h>

#ifdef _USE_LINUX_
#include <dlfcn.h>
#include <fcntl.h>
#include <liburing.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

typedef pthread_t mthread_t;
typedef pthread_mutex_t mmutex_t;
typedef pthread_cond_t mcond_t;
typedef int mdataline_t;
typedef int mfd_t;
typedef void *mmap_t;
typedef mptr_t mdlentry_t;

#else
// others....
#endif

#endif
