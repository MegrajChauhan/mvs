#ifndef _MVS_REQUEST_INTERFACE_
#define _MVS_REQUEST_INTERFACE_

#include <mvs_entity_interface.h>
#include <mvs_protectors.h>
#include <mvs_tools.h>
#include <mvs_types.h>
#include <mvs_int_result.h>

_MVS_ATTR_EXTERNAL_ MVSIntResult
mvs_request_check_status(struct MVSGravesRequest *req);

_MVS_ATTR_EXTERNAL_ MVSIntResult mvs_request_get_result(struct MVSGravesRequest *req);

_MVS_ATTR_EXTERNAL_ MVSIntResult
mvs_create_req_SPAWN_ENTITY(MVSEntityIdentityHdlr iden, mcond_t *cond,
                            msize_t ID, mqword_t config, mqword_t properties, struct MVSGravesRequest **req);

#endif
