#include <merry_core_request_setup.h>

#define _MCREQSETUP_(req) merry_core_req_setup##req

typedef struct MVSGravesRequest* (*mcreqsetup_t)(MerryCore *);

mcreqsetup_t SETUP_FUNCS[] = {
	_MCREQSETUP_(spawn_entity),
};

_MVS_ATTR_INTERNAL_ void merry_map_res_to_regr(MerryCore *c, MVSIntResult *res) {
		c->REGISTER_FILE[MERRY_CORE_R15] = !res->status;
		c->REGISTER_FILE[MERRY_CORE_R14] = res->src;
		c->REGISTER_FILE[MERRY_CORE_R13] = res->code;
}

/*
 * R15: represents the status of the request
 * R14: The ID of the request if correctly added else ERRNO if any
 */
/*mbool_t merry_core_handle_async_requests(MerryCore *core) {
  register mqword_t req_num = core->REGISTER_FILE[MERRY_CORE_R15];
  if (req_num >= GREQ_COUNT) {
  	MERR("Unknown MVS Request: REQ_ID=%zu", req_num);
  	core->REGISTER_FILE[MERRY_CORE_R15] = MERRY_RESULT_INVALID_MREQ;
  	return mtrue;
  }
  if (merry_is_list_full(core->req_list)) {
    MLOG("ASYNC REQ Queue full: REQ_ID=%zu", req_num);
 	core->REGISTER_FILE[MERRY_CORE_R15] = MRES_SETUP_FAILED;
 	return mtrue;	
  }
  MerryGravesRequest *req = (MerryGravesRequest*)malloc(sizeof(MerryGravesRequest));
  if (!req) {
  	MERR("Failed to INITIALIZE a MERRY REQUEST(Memory allocation failure): REQ_ID=%zu", req_num);
  	core->REGISTER_FILE[MERRY_CORE_R15] = MRES_SETUP_FAILED;
    core->REGISTER_FILE[MERRY_CORE_R14] = errno;
  	return mtrue;
  }
  if (merry_cond_init(&req->cond) != MRES_SUCCESS) {
  	MERR("Failed to INITIALIZE a MERRY REQUEST(Metadata allocation failure): REQ_ID=%zu", req_num);
  	core->REGISTER_FILE[MERRY_CORE_R15] = MRES_SETUP_FAILED;
  	core->REGISTER_FILE[MERRY_CORE_R14] = errno;
  	return mtrue;
  }
  req->type = req_num;
  req->async = mtrue;
  mbool_t res = SETUP_FUNCS[req_num](core, req);
  if (!res)
    return mfalse;
  // ready to be added to the QUEUE
  if (merry_Request_list_push(core->req_list, &req) != MRES_SUCCESS) {
  	MERR("Failed to INITIALIZE a MERRY REQUEST(ASYNC REQ QUEUE Failure): REQ_ID=%zu", req_num);
  	core->REGISTER_FILE[MERRY_CORE_R15] = MRES_SETUP_FAILED;
  	core->REGISTER_FILE[MERRY_CORE_R14] = errno;
  	return mtrue;  	
  }
  core->REGISTER_FILE[MERRY_CORE_R15] = MRES_SUCCESS;
  core->REGISTER_FILE[MERRY_CORE_R14] = merry_Reqe
  return mtrue;
}*/

mbool_t merry_core_handle_sync_requests(MerryCore *core) {
  register mqword_t req_num = core->REGISTER_FILE[MERRY_CORE_R15];
  if (req_num >= GREQ_COUNT) {
  	MERR("Unknown MVS Request: REQ_ID=%zu", req_num);
  	core->REGISTER_FILE[MERRY_CORE_R15] = MERRY_RESULT_INVALID_MREQ;
  	return mtrue;
  }
  MVSGravesRequest *req = SETUP_FUNCS[req_num](core, NULL);
  if (!req)
		return mtrue;
  res = core->ctx.API.make_request(req);
  if (!res.status) {
		merry_map_res_to_regr(core, &res);
		free(req);
		return NULL;
  }
  c->REGISTER_FILE[MERRY_CORE_R15] = 0;
  c->REGISTER_FILE[MERRY_CORE_R14] = 0;
  c->REGISTER_FILE[MERRY_CORE_R13] = 0;
  return mtrue;
}

merry_core_req_setupREQ(spawn_entity) {
		MVSGravesRequest *req;
		mqword_t conf, prop;
		mqword_t EID = core->REGISTER_FILE[MERRY_CORE_R14];
		conf = core->REGISTER_FILE[MERRY_CORE_R13];
		prop = core->REGISTER_FILE[MERRY_CORE_R12];
        
        MVSIntResult res = mvs_create_req_SPAWN_ENTITY(core->ctx.self, cond, EID, conf, prop, &req);
		if (!res.status) {
				merry_map_res_to_regr(core, &res);
				return NULL;
		}
		return req;
}
