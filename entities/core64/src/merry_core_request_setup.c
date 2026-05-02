#include <merry_core_request_setup.h>

#define _MCREQSETUP_(req) merry_core_req_setup##req

_MERRY_DEFINE_FUNC_PTR_(mbool_t, mcreqsetup_t, MerryCore *, MerryGravesRequest*);

mcreqsetup_t SETUP_FUNCS[] = {
	_MCREQSETUP_(create_core),
	_MCREQSETUP_(create_group),
	_MCREQSETUP_(get_system_details)
};

/*
 * R15: represents the status of the request
 * R14: The ID of the request if correctly added else ERRNO if any
 */
mbool_t merry_core_handle_async_requests(MerryCore *core) {
  register mqword_t req_num = core->REGISTER_FILE[MERRY_CORE_R15];
  if (req_num >= GREQ_COUNT) {
  	MERR("Unknown Merry Request ID: REQ_ID=%zu", req_num);
  	core->REGISTER_FILE[MERRY_CORE_R15] = MRES_UNRECOGNIZED;
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
}

mbool_t merry_core_handle_sync_requests(MerryCore *core);

merry_core_req_setupREQ(create_core);
merry_core_req_setupREQ(create_group);
merry_core_req_setupREQ(get_system_details);
