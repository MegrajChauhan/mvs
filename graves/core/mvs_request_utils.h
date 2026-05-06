#ifndef _MVS_REQUEST_UTILS_
#define _MVS_REQUEST_UTILS_

#include <mvs_graves_constants.h>
#include <mvs_int_result.h>
#include <mvs_logger.h>
#include <mvs_request.h>
#include <mvs_request_types.h>
#include <mvs_types.h>
#include <stdatomic.h>
#include <string.h>

MVSIntResult mvs_request_check_status(MVSGravesRequest *req);

MVSIntResult mvs_request_get_result(MVSGravesRequest *req);

/*
 * In case of every request, if the condition variable is NULL than the request
 * is automatically inferenced as an async request.
 */

MVSIntResult mvs_create_req_SPAWN_ENTITY(MVSEntityIdentity *iden,
                                              mcond_t *cond, msize_t ID,
                                              mqword_t config,
                                              mqword_t properties,
											  MVSGravesRequest **req);

#endif
