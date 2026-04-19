#ifndef _MVS_FILE_DEFS_
#define _MVS_FILE_DEFS_

#include <mvs_platform.h>
#include <mvs_interface_flags.h>

// Opening modes
#define MVS_FILE_MODE_APPEND  (1 << 0)
#define MVS_FILE_MODE_CREATE  (1 << 1)
#define MVS_FILE_MODE_READ  (1 << 2)
#define MVS_FILE_MODE_WRITE  (1 << 3)
#define MVS_FILE_MODE_READ_WRITE (1 << 4)

#ifdef _USE_LINUX_

// For file access permissions regarding various users, groups, and such, the same might not apply to all platforms, thus, for now, we will be using a workaround and giving default
// permissions
#define MVS_FILE_MODE_PERMISSIONS (S_IRWXU | S_IRWXG | S_IRWXO) // by default, the access permission is given to everyone
// excuse the naming

#else
// not yet
#endif

#endif
