#ifndef _MVS_REQUEST_
#define _MVS_REQUEST_

#include <api_request.h>
#include <api_request_response.h>
#include <mvs_entity.h>
#include <mvs_graves_constants.h>
#include <mvs_protectors.h>
#include <mvs_request_types.h>
#include <mvs_types.h>
#include <stdatomic.h>
#include <stdlib.h>

typedef struct MVSGravesRequest MVSGravesRequest;

struct MVSGravesRequest {
  MVSEntityIdentity *iden;
  mRequest_t type;
  mcond_t *wakeup_cond;
  atm_mbool_t request_served;
  atm_mbool_t queued;
  APIRequestResponse response;
  GravesRequestResult result;
  union {
    struct {
      msize_t ID;
      mqword_t config;
      mqword_t properties;
      mqword_t internal_conf; // 0 would imply 'default'
    } spawn_entity;
  } args;
};

#endif
