#include <mvs_request_utils.h>

_MVS_ATTR_EXPORT_
apiRes_t mvs_request_check_status(MVSGravesRequest *req) {
  if (!req || !atomic_load_explicit(&req->queued, memory_order_relaxed))
    return API_RES_INVALID_ARGS;
  if (!atomic_load_explicit(&req->request_served, memory_order_relaxed))
    return API_RES_REQ_NOT_SERVED;
  return API_RES_REQ_SERVED;
}

_MVS_ATTR_EXPORT_
apiRes_t mvs_request_get_response(MVSGravesRequest *req,
                                  APIRequestResponse *res) {
  if (!req || !res || !atomic_load_explicit(&req->queued, memory_order_relaxed))
    return API_RES_INVALID_ARGS;
  if (!atomic_load_explicit(&req->request_served, memory_order_relaxed))
    return API_RES_REQ_NOT_SERVED;
  *res = req->response;
  return API_RES_SUCCESS;
}

_MVS_ATTR_EXPORT_
apiRes_t mvs_request_get_result(MVSGravesRequest *req,
                                GravesRequestResult *res) {
  if (!req || !res || !atomic_load_explicit(&req->queued, memory_order_relaxed))
    return API_RES_INVALID_ARGS;
  if (!atomic_load_explicit(&req->request_served, memory_order_relaxed))
    return API_RES_REQ_NOT_SERVED;
  *res = req->result;
  return API_RES_SUCCESS;
}

_MVS_ATTR_EXPORT_
apiRes_t mvs_create_req_SPAWN_ENTITY(MVSEntityIdentity *iden, msize_t ID,
                                     mqword_t config, mqword_t properties,
                                     mqword_t in_conf, MVSGravesRequest **req) {
  if (!iden || !req)
    return API_RES_INVALID_ARGS;
  mvs_log_dbg("Request Create: SPAWN_ENTITY, entity[ID=%zu, UID=%zu]", iden->ID,
              iden->UID);
  // The verification of the configuration, properties, and identity is Graves'
  // job
  MVSGravesRequest *r = (MVSGravesRequest *)malloc(sizeof(MVSGravesRequest));
  if (!r) {
    mvs_log_err("Request Create(FAILED): SPAWN_ENTITY, entity[ID=%zu, "
                "UID=%zu]: Memory Allocation Failure",
                iden->ID, iden->UID);
    return API_RES_NO_MEM;
  }
  r->type = MREQ_SPAWN_ENTITY;
  r->iden = iden;
  atomic_init(&r->queued, mfalse);
  r->args.spawn_entity.ID = ID;
  r->args.spawn_entity.config = config;
  r->args.spawn_entity.properties = properties;
  r->args.spawn_entity.internal_conf = in_conf;
  mvs_log_dbg("Request Create(SUCCESS): SPAWN_ENTITY, entity[ID=%zu, UID=%zu]",
              iden->ID, iden->UID);
  *req = r;
  return API_RES_SUCCESS;
}
