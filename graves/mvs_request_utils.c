#include <mvs_request_utils.h>

APIResult mvs_request_check_status(MVSGravesRequest *req) {
  if (!req || !atomic_load_explicit(&req->queued, memory_order_relaxed))
    return _API_BAD_CALL_(API_RES_INVALID_ARGS);
  if (!atomic_load_explicit(&req->request_served, memory_order_relaxed))
    return _API_SUCCESS_(API_SRC_GRAVES, API_CODE_REQ_NOT_SERVED);
  return _API_SUCCESS_(API_SRC_GRAVES, API_CODE_REQ_SERVED);
}

APIResult mvs_request_get_response(MVSGravesRequest *req, APIRequestResponse *res) {
  if (!req || !res ||
      !atomic_load_explicit(&req->queued, memory_order_relaxed))
    return _API_BAD_CALL_(API_RES_INVALID_ARGS);
  if (!atomic_load_explicit(&req->request_served, memory_order_relaxed))
    return _API_SUCCESS_(API_SRC_GRAVES, API_CODE_REQ_NOT_SERVED);
  *res = req->response;
  return _API_GOOD_CALL_();
}

APIResult mvs_create_req_SPAWN_ENTITY(MVSEntityIdentity *iden,
                                              mcond_t *cond, msize_t ID,
                                              mqword_t config,
                                              mqword_t properties,
											  mqword_t in_conf,
											  MVSGravesRequest **req) {
  if (!iden || !req)
    return _API_BAD_CALL_(API_RES_INVALID_ARGS);
  mvs_log_dbg("Request Create: SPAWN_ENTITY, entity[ID=%zu, UID=%zu]", iden->ID,
              iden->UID);
  // The verification of the configuration, properties, and identity is Graves'
  // job
  MVSGravesRequest *r = (MVSGravesRequest *)malloc(sizeof(MVSGravesRequest));
  if (!r) {
    mvs_log_err("Request Create(FAILED): SPAWN_ENTITY, entity[ID=%zu, "
                "UID=%zu]: Memory Allocation Failure",
                iden->ID, iden->UID);
    return _API_FAILURE_(API_SRC_HOST, errno);
  }
  r->type = MREQ_SPAWN_ENTITY;
  r->iden = iden;
  r->wakeup_cond = cond;
  atomic_init(&r->queued, mfalse);
  r->args.spawn_entity.ID = ID;
  r->args.spawn_entity.config = config;
  r->args.spawn_entity.properties = properties;
  r->args.spawn_entity.internal_conf = in_conf;
  mvs_log_dbg("Request Create(SUCCESS): SPAWN_ENTITY, entity[ID=%zu, UID=%zu]",
              iden->ID, iden->UID);
  *req = r;
  return _API_GOOD_CALL_();
}
