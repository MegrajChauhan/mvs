#ifndef _API_REQUEST_
#define _API_REQUEST_

#include <api_types.h>
#include <api_request_response.h>
#include <api_entity.h>
#include <mvs_protectors.h>
#include <mvs_types.h>

typedef union GravesRequestResult GravesRequestResult;

union GravesRequestResult {
   struct {
       msize_t NOTHING;
   } spawn_entity;
};

apiRes_t api_request_check_status(GravesRequest *req);

apiRes_t api_request_get_response(GravesRequest *req, APIRequestResponse *res); 

apiRes_t api_request_get_result(GravesRequest *req, GravesRequestResult *res);

apiRes_t api_request_create_SPAWN_ENTITY(EntityIdentityHdlr hdlr, mcond_t *cond, msize_t EID, mqword_t config, mqword_t properties, mqword_t in_conf, GravesRequest **req); 

#endif
