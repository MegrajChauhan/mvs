#ifndef _MVS_SLIST_NODE_
#define _MVS_SLIST_NODE_

#include <mvs_list.h>
#include <mvs_types.h>

typedef struct MVSSlistCommand MVSSlistCommand;

struct MVSSlistCommand {
  mbool_t copy;
  msize_t EID;
  msize_t config;
  msize_t properties;
  msize_t setup;
  MVSDynamicListLinear *args;
  MVSDynamicListLinear *local_list;
  struct {
    msize_t local_list_lim;
  } limits;
};

#endif
