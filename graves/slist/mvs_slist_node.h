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
  mbool_t config_provided, properties_provided, setup_provided;
  mstr_t args;
  MVSDynamicListLinear *local_list;
  mbool_t skip;
  mbool_t launch_success;
  msize_t ID_after_launch;
  struct {
    msize_t local_list_lim;
  } limits;
};

#endif
