#ifndef _MVS_REQUEST_
#define _MVS_REQUEST_

#include <mvs_request_types.h>
#include <mvs_types.h>
#include <mvs_entity.h>
#include <mvs_entity_interface.h>
#include <mvs_protectors.h>
#include <mvs_graves_constants.h>
#include <stdlib.h>
#include <stdatomic.h>

typedef struct MVSGravesRequest MVSGravesRequest;

struct MVSGravesRequest {
    MVSEntityIdentity* iden;
	mRequest_t type;
	mcond_t *wakeup_cond;
	atm_mbool_t request_served;
	atm_mbool_t queued;
	msize_t RESULT[_MVS_CONSTANT_REQUEST_RESULT_ARRAY_LEN_]; // This where the result is provided for the request
	union {
		struct {
		    msize_t ID;
			mqword_t config;
			mqword_t properties;
		} spawn_entity;
	} args;
};

#endif
