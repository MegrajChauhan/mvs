#ifndef _MVS_REQUEST_UTILS_
#define _MVS_REQUEST_UTILS_

#include <mvs_graves_constants.h>
#include <api_results.h>
#include <api_request.h>
#include <api_exposed_graves_code.h>
#include <mvs_logger.h>
#include <mvs_request.h>
#include <mvs_request_types.h>
#include <mvs_types.h>
#include <stdatomic.h>
#include <string.h>

apiRes_t mvs_request_check_status(MVSGravesRequest *req);

apiRes_t mvs_request_get_response(MVSGravesRequest *req, APIRequestResponse *res);

apiRes_t mvs_request_get_result(MVSGravesRequest *req, GravesRequestResult *res);
/*
 * In case of every request, if the condition variable is NULL than the request
 * is automatically inferenced as an async request.
 */

apiRes_t mvs_create_req_SPAWN_ENTITY(MVSEntityIdentity *iden,
                                              mcond_t *cond, msize_t ID,
                                              mqword_t config,
                                              mqword_t properties,
											  mqword_t in_conf,
											  MVSGravesRequest **req);

#endif
