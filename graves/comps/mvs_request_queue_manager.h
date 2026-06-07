#ifndef _MVS_REQUEST_QUEUE_MANAGER_
#define _MVS_REQUEST_QUEUE_MANAGER_

#include <errno.h>
#include <mvs_barrier.h>
#include <mvs_graves_constants.h>
#include <mvs_logger.h>
#include <mvs_protectors.h>
#include <mvs_queue.h>
#include <mvs_request.h>
#include <mvs_request_types.h>
#include <mvs_types.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>

typedef struct MVSRequestQueueManager MVSRequestQueueManager;

struct MVSRequestQueueManager {
  MVSDynamicQueueLinear *request_queue;
  mmutex_t lock;
};

MVSRequestQueueManager *mvs_request_queue_manager_create();
void mvs_request_queue_manager_destroy(MVSRequestQueueManager *req_manager);

/*
 * Return:
 * 0 = success
 * 1 = Failure
 */
msize_t
mvs_request_queue_manager_enqueue_request(MVSRequestQueueManager *req_manager,
                                          MVSGravesRequest *req);

MVSGravesRequest *
mvs_request_queue_manager_dequeue_request(MVSRequestQueueManager *req_manager);

#endif
