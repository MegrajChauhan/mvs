#include <mvs_request_utils.h>

MVSIntResult mvs_request_check_status(MVSGravesRequest *req) {
  if (!req || !atomic_load_explicit(&req->queued, memory_order_relaxed))
    return _MVS_MFUNC_MAKE_INT_RESULT_(mfalse, MINT_SRC_GRAVES, _MVS_CONSTANT_REQUEST_REQ_INVALID_ARGS_);
  if (!atomic_load_explicit(&req->request_served, memory_order_relaxed))
    return _MVS_MFUNC_MAKE_INT_RESULT_(mtrue, MINT_SRC_GRAVES, _MVS_CONSTANT_REQUEST_REQ_NOT_SERVED_);
  return _MVS_MFUNC_MAKE_INT_RESULT_(mtrue, MINT_SRC_GRAVES, _MVS_CONSTANT_REQUEST_REQ_SERVED_);
}

MVSIntResult mvs_request_get_result(MVSGravesRequest *req) {
  if (!req || !result ||
      !atomic_load_explicit(&req->queued, memory_order_relaxed))
    return _MVS_MFUNC_MAKE_INT_RESULT_(mfalse, MINT_SRC_GRAVES, _MVS_CONSTANT_REQUEST_REQ_INVALID_ARGS_);
  if (!atomic_load_explicit(&req->request_served, memory_order_relaxed))
    return _MVS_MFUNC_MAKE_INT_RESULT_(mtrue, MINT_SRC_GRAVES, _MVS_CONSTANT_REQUEST_REQ_NOT_SERVED_);
  return req->res;
}

MVSIntResult mvs_create_req_SPAWN_ENTITY(MVSEntityIdentity *iden,
                                              mcond_t *cond, msize_t ID,
                                              mqword_t config,
                                              mqword_t properties, MVSGravesRequest **req) {
  if (!iden)
    return _MVS_MFUNC_MAKE_INT_RESULT_(mfalse, MINT_SRC_GRAVES, _MVS_CONSTANT_REQUEST_REQ_INVALID_ARGS_);
  mvs_log_dbg("Request Create: SPAWN_ENTITY, entity[ID=%zu, UID=%zu]", iden->ID,
              iden->UID);
  // The verification of the configuration, properties, and identity is Graves'
  // job
  MVSGravesRequest *r = (MVSGravesRequest *)malloc(sizeof(MVSGravesRequest));
  if (!r) {
    mvs_log_err("Request Create(FAILED): SPAWN_ENTITY, entity[ID=%zu, "
                "UID=%zu]: Memory Allocation Failure",
                iden->ID, iden->UID);
    return NULL;
    return _MVS_MFUNC_MAKE_INT_RESULT_(mfalse, MINT_SRC_HOST, _MVS_CONSTANT_REQUEST_REQ_SYS_FAILURE_);
  }
  r->type = MREQ_SPAWN_ENTITY;
  r->iden = iden;
  r->wakeup_cond = cond;
  atomic_init(&r->queued, mfalse);
  r->args.spawn_entity.ID = ID;
  r->args.spawn_entity.config = config;
  r->args.spawn_entity.properties = properties;
  mvs_log_dbg("Request Create(SUCCESS): SPAWN_ENTITY, entity[ID=%zu, UID=%zu]",
              iden->ID, iden->UID);
  *req = r;
  return _MVS_MFUNC_MAKE_INT_RESULT_(mtrue, MINT_SRC_GRAVES, _MVS_CONSTANT_REQUEST_REQ_SUCCESS_);
}
