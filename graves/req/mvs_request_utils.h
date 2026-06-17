#ifndef _MVS_REQUEST_UTILS_
#define _MVS_REQUEST_UTILS_

#include <api_results.h>
#include <mvs_graves_constants.h>
#include <mvs_logger.h>
#include <mvs_request.h>
#include <mvs_request_types.h>
#include <mvs_types.h>
#include <stdatomic.h>
#include <string.h>

_MVS_ATTR_EXPORT_ apiRes_t mvs_request_check_status(MVSGravesRequest *req);

_MVS_ATTR_EXPORT_ apiRes_t mvs_request_get_response(MVSGravesRequest *req,
                                                    APIRequestResponse *res);
_MVS_ATTR_EXPORT_
apiRes_t mvs_request_get_result(MVSGravesRequest *req,
                                GravesRequestResult *res);
/*
 * In case of every request, if the condition variable is NULL than the request
 * is automatically inferenced as an async request.
 */
_MVS_ATTR_EXPORT_
apiRes_t mvs_create_req_SPAWN_ENTITY(MVSEntityIdentity *iden, msize_t ID,
                                     mqword_t config, mqword_t properties,
                                     mqword_t in_conf, MVSGravesRequest **req);

#endif
