#include <mvs_request_queue_manager.h>

MVSRequestQueueManager *mvs_request_queue_manager_create() {
  mvs_log_dbg("Initializing Request Queue Manager...");
  MVSRequestQueueManager *rman =
      (MVSRequestQueueManager *)malloc(sizeof(MVSRequestQueueManager));
  if (!rman) {
    mvs_log_err("Failed to initialize Request Queue Manager: Memory Allocation "
                "Failure[%s]",
                strerror(errno));
    return NULL;
  }
  mResult_t res;
  if ((res = mvs_dynamic_lqueue_create(
           &rman->request_queue, _MVS_CONSTANT_REQUEST_QUEUE_INITIAL_LENGTH_,
           sizeof(MVSGravesRequest *))) != MRES_SUCCESS) {
    /// TODO: Need strerror for mResult_t too
    mvs_log_err("Failed to initialize Request Queue Manager: Memory Allocation "
                "Failure[%s]",
                strerror(errno));
    free(rman);
    return NULL;
  }
  if ((res = mvs_mutex_init(&rman->lock)) != MRES_SUCCESS) {
    mvs_log_err("Failed to initialize Request Queue Manager: Memory Allocation "
                "Failure[%s]",
                strerror(errno));
    mvs_dynamic_lqueue_destroy(rman->request_queue);
    free(rman);
    return NULL;
  }
  mvs_log_dbg("Successfully Initialized Request Queue Manager");
  return rman;
}

void mvs_request_queue_manager_destroy(MVSRequestQueueManager *req_manager) {
  mvs_log_dbg("Destroying Request Queue Manager");
  mvs_dynamic_lqueue_destroy(req_manager->request_queue);
  mvs_mutex_destroy(&req_manager->lock);
  free(req_manager);
  mvs_log_dbg("Destroyed Request Queue Manager");
}

msize_t
mvs_request_queue_manager_enqueue_request(MVSRequestQueueManager *req_manager,
                                          MVSGravesRequest *req) {
  msize_t ret = 0;
  mvs_mutex_lock(&req_manager->lock);
  mResult_t res;
  if ((res = mvs_dynamic_lqueue_enqueue(req_manager->request_queue, &req)) !=
      MRES_SUCCESS) {
    ret = 1;
  } else {
    atomic_store_explicit(&req->request_served, mfalse, memory_order_release);
    atomic_store_explicit(&req->queued, mtrue, memory_order_release);
    mvs_cond_wait(req->wakeup_cond, &req_manager->lock);
  }
  mvs_mutex_unlock(&req_manager->lock);
  return ret;
}

msize_t mvs_request_queue_manager_enqueue_request_async(
    MVSRequestQueueManager *req_manager, MVSGravesRequest *req) {
  msize_t ret = 0;
  mvs_mutex_lock(&req_manager->lock);
  mResult_t res;
  if ((res = mvs_dynamic_lqueue_enqueue(req_manager->request_queue, &req)) !=
      MRES_SUCCESS) {
    ret = 1;
  }
  atomic_store_explicit(&req->request_served, mfalse, memory_order_release);
  atomic_store_explicit(&req->queued, mtrue, memory_order_release);
  mvs_mutex_unlock(&req_manager->lock);
  return ret;
}

MVSGravesRequest *
mvs_request_queue_manager_dequeue_request(MVSRequestQueueManager *req_manager) {
  MVSGravesRequest *req;
  if (mvs_dynamic_lqueue_dequeue(req_manager->request_queue, &req) !=
      MRES_SUCCESS)
    return NULL;
  return req;
}
