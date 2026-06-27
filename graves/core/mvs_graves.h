#ifndef _MVS_GRAVES_
#define _MVS_GRAVES_

#include <api_entity.h>
#include <api_graves.h>
#include <api_results.h>
#include <mvs_arg_parse.h>
#include <mvs_barrier.h>
#include <mvs_entity.h>
#include <mvs_entity_registry.h>
#include <mvs_graves_entity_list.h>
#include <mvs_graves_entity_utils.h>
#include <mvs_interface.h>
#include <mvs_list.h>
#include <mvs_logger.h>
#include <mvs_protectors.h>
#include <mvs_request_queue_manager.h>
#include <mvs_request_utils.h>
#include <mvs_rlist.h>
#include <mvs_slist.h>
#include <mvs_system_config.h>
#include <mvs_threads.h>
#include <mvs_types.h>
#include <stdlib.h>

typedef struct MVSGraves MVSGraves;
typedef struct MVSGravesAccounting MVSGravesAccounting;
typedef struct MVSGravesSync MVSGravesSync;
typedef struct MVSGravesComps MVSGravesComps;
typedef struct MVSGravesState MVSGravesState;

struct MVSGravesAccounting {
  msize_t requests_served;
  msize_t requests_success;
};

struct MVSGravesSync {
  /*
   * the following barrier is used to ensure all entities launch at startup
   * */
  MVSBarrier sleep_barrier;
  MVSBarrier release_barrier;
};

struct MVSGravesComps {
  MVSRlist rlist;
  MVSSlist slist;
  MVSGravesEntityList *entity_list;
  MVSRequestQueueManager *queue_manager;
};

struct MVSGravesState {
  MVSArgParseResult cmd_opts;
  MVSSystemConfig config;
  mbool_t all_initial_entities_initialized;
  mbool_t all_initial_entities_launched;
};

struct MVSGraves {
  MVSGravesAccounting accounting;
  MVSGravesSync sync;
  MVSGravesComps components;
  MVSGravesState state;
};

/*
 * Exposed API(This will not return and exit internally)
 */
void mvs_run(mstr_t *argv, msize_t argc);

/*
 * API functions
 * */
_MVS_ATTR_EXPORT_
apiRes_t mvs_graves_make_request(MVSEntityIdentity *iden, GravesRequest **req);

#endif
