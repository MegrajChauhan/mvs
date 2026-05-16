#include <api_request.h>
#include <mvs_request_utils.h>

APIResult api_request_check_status(GravesRequest *req) {
		return mvs_request_check_status(req);
}

APIResult api_request_get_response(GravesRequest *req, APIRequestResponse *res) {
		return mvs_request_get_response(req, res);
} 

APIResult api_request_create_SPAWN_ENTITY(EntityIdentityHdlr hdlr, mcond_t *cond, 
				msize_t EID, mqword_t config, mqword_t properties,mqword_t in_conf, GravesRequest **req) {
		return mvs_create_req_SPAWN_ENTITY(hdlr, cond, EID, config, properties,in_conf, req);
} 


