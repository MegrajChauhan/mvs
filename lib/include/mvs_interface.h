#ifndef _MVS_INTERFACE_
#define _MVS_INTERFACE_

#include <mvs_helpers.h>
#include <mvs_interface_types.h>
#include <mvs_platform.h>
#include <mvs_results.h>
#include <mvs_types.h>
#include <stdlib.h>

typedef struct MVSInterface MVSInterface;

struct MVSInterface {
  mInterface_t interface;
  union {
    struct {
      mfd_t fd;
      struct {
        mbyte_t file_opened : 1;
        mbyte_t read : 1;
        mbyte_t write : 1;
        mbyte_t append : 1;
        mbyte_t resb : 4;
      } flags;
    } file;
  };
};

mResult_t mvs_interface_init(MVSInterface **interface, mInterface_t type);

mResult_t mvs_interface_destroy(MVSInterface *interface);

#endif
