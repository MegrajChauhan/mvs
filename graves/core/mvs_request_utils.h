#ifndef _MVS_REQUEST_UTILS_
#define _MVS_REQUEST_UTILS_

#include <mvs_graves_constants.h>
#include <mvs_logger.h>
#include <mvs_request.h>
#include <mvs_request_types.h>
#include <mvs_types.h>
#include <stdatomic.h>
#include <string.h>

/*
 * Return:
 * 0 = Not served
 * 1 = Served
 * 2 = Invalid request(req == NULL || req is not queued yet)
 */
msize_t mvs_request_check_status(MVSGravesRequest *req);

/*
 * Return:
 * 0 = Success(the result is copied into 'result')
 * 1 = Request hasn't been served
 * 2 = Request invalid or 'result' is NULL
 */
msize_t mvs_request_get_result(MVSGravesRequest *req, msize_t *result);

/*
 * In case of every request, if the condition variable is NULL than the request
 * is automatically inferenced as an async request.
 */

MVSGravesRequest *mvs_create_req_SPAWN_ENTITY(MVSEntityIdentity *iden,
                                              mcond_t *cond, msize_t ID,
                                              mqword_t config,
                                              mqword_t properties);

#endif
