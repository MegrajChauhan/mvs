#ifndef _MVS_GRAVES_
#define _MVS_GRAVES_

#include <mvs_arg_parse.h>
#include <mvs_entity.h>
#include <mvs_entity_interface.h>
#include <mvs_entity_registry.h>
#include <mvs_interface.h>
#include <mvs_list.h>
#include <mvs_logger.h>
#include <mvs_protectors.h>
#include <mvs_request_queue_manager.h>
#include <mvs_request_result.h>
#include <mvs_threads.h>
#include <mvs_types.h>
#include <stdlib.h>

typedef struct MVSGraves MVSGraves;

struct MVSGraves {
  msize_t entity_count;    // The number of entities, active and dead, in total
  msize_t active_entities; // All of the active entities
  msize_t return_val;

  mcond_t graves_cond;
  mmutex_t graves_lock;

  MVSArgParseResult *cmd_opts; // Command-line arguments provided
  MVSDynamicListLinear
      *entity_list; // all of the entities; active and non-active
  MVSRequestQueueManager *req_queue;

  MVSGravesAPI local_API; // For Local entities(I will need to define another
                          // one for external entities)
};

/*
 * Exposed API(This will not return and exit internally)
 */
void mvs_run(MVSArgParseResult *opts);

/*
 * Here, i have decided to expose the functions that comprise Manager API for
 * the entities.
 */

/*
 * Returns:
 * 0 = Success
 * 1 = Failed
 * 2 = Invalid Request due to invalid argument(s)
 */
msize_t mvs_graves_API_make_request(MVSEntityIdentity *hdlr,
                                    MVSGravesRequest *req);

#endif
