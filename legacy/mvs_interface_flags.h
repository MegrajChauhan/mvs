#ifndef _MVS_INTERFACE_FLAGS_
#define _MVS_INTERFACE_FLAGS_

// Configurations
#define MVS_INTERFACE_CONF_SHAREABLE (1 << 0) // Make the interface shareable

// State flags
#define MVS_INTERFACE_STATE_INITIALIZED                                        \
  (1 << 0) // the interface has been initialized
#define MVS_INTERFACE_STATE_SHARED                                             \
  (1 << 1) // the interface we have was shared and not the original one

// Flags
// #define MVS_INTERFACE_FLAG_COPY_ON_SHARE (1 << 0)

#endif
