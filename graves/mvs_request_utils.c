#include <mvs_request_utils.h>

msize_t mvs_request_check_status(MVSGravesRequest* req) {
	if (!req || !atomic_load_explicit(&req->queued, memory_order_relaxed))
	   return 2;
	if (!atomic_load_explicit(&req->request_served, memory_order_relaxed))
	   return 0;
	return 1;
}

msize_t mvs_request_get_result(MVSGravesRequest *req, msize_t *result) {
	if (!req || !result || !atomic_load_explicit(&req->queued, memory_order_relaxed))
	   return 2;
	if (!atomic_load_explicit(&req->request_served, memory_order_relaxed))
	   return 1;
	memcpy(result, req->RESULT, _MVS_CONSTANT_REQUEST_RESULT_ARRAY_LEN_ * sizeof(msize_t));
	return 0;
}

MVSGravesRequest* mvs_create_req_SPAWN_ENTITY(MVSEntityIdentity *iden, mcond_t *cond, msize_t ID, mqword_t config, mqword_t properties) {
	if (!iden)
	   return NULL;
    mvs_log_dbg("Request Create: SPAWN_ENTITY, entity[ID=%zu, UID=%zu]", iden->ID, iden->UID);
    // The verification of the configuration, properties, and identity is Graves' job
    MVSGravesRequest *req = (MVSGravesRequest*)malloc(sizeof(MVSGravesRequest));
    if (!req) {
	   mvs_log_err("Request Create(FAILED): SPAWN_ENTITY, entity[ID=%zu, UID=%zu]: Memory Allocation Failure", iden->ID, iden->UID);	
       return NULL;
    }
    req->type = MREQ_SPAWN_ENTITY;
    req->iden = iden;
    req->wakeup_cond = cond;
    atomic_init(&req->queued, mfalse);
    req->args.spawn_entity.ID = ID;
    req->args.spawn_entity.config = config;
    req->args.spawn_entity.properties = properties;
    mvs_log_dbg("Request Create(SUCCESS): SPAWN_ENTITY, entity[ID=%zu, UID=%zu]", iden->ID, iden->UID);
    return req;
}
