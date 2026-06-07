#ifndef _MVS_GRAVES_
#define _MVS_GRAVES_

#include <api_entity.h>
#include <api_graves.h>
#include <api_results.h>
#include <mvs_arg_parse.h>
#include <mvs_barrier.h>
#include <mvs_entity.h>
#include <mvs_entity_registry.h>
#include <mvs_graves_arg_parse.h>
#include <mvs_graves_entity_list.h>
#include <mvs_graves_entity_utils.h>
#include <mvs_interface.h>
#include <mvs_list.h>
#include <mvs_logger.h>
#include <mvs_protectors.h>
#include <mvs_request_queue_manager.h>
#include <mvs_slist.h>
#include <mvs_threads.h>
#include <mvs_types.h>
#include <mvs_rlist.h>
#include <mvs_system_config.h>
#include <mvs_request_utils.h>
#include <stdlib.h>

typedef struct MVSGraves MVSGraves;

struct MVSGraves {
  msize_t return_val;
  msize_t entity_created;
  mcond_t graves_cond;
  mmutex_t graves_lock;
  MVSBarrier wait_barrier;
  MVSBarrier sync_barrier;
  mbool_t all_launch_success;
  MVSSlist slist;
  MVSRlist *rlist;
  MVSArgParseResult *cmd_opts; // Command-line arguments provided
  MVSGravesEntityList
      *entity_list; // all of the entities; active and non-active
  MVSRequestQueueManager *req_queue;
  GravesAPI local_API; // For Local entities(I will need to define another
                       // one for external entities)
  MVSSystemConfig *config;
};

/*
 * Exposed API(This will not return and exit internally)
 */
void mvs_run(MVSArgParseResult *opts, MVSRlist *rlist, MVSSystemConfig *conf);

/*
 * API functions
 * */
_MVS_ATTR_EXPORT_
apiRes_t mvs_graves_make_request(MVSEntityIdentity *iden, GravesRequest **req);

#endif
