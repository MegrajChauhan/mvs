#ifndef _API_ENTITY_
#define _API_ENTITY_

#include <api_results.h>
#include <api_request.h>
#include <api_types.h>

typedef struct EntityContext EntityContext;
typedef struct GravesAPI GravesAPI;

/*
 * The entity may use this API call to push a request.
 * The request may be synchronous or asynchronous based on if a wakeup condition is provided or not.
 * See the request API for more information.
 * If the request is synchronous, the call will not return until the request has been served else, it will return immediately.
 * If pushing the request fails, then also the call will return immediately. The status of the API call will be returned as APIResult
 * */
typedef APIResult (*gravesmr_t)(EntityIdentityHdlr, GravesRequest **);

struct GravesAPI {
    gravesmr_t make_request;
};

struct EntityContext {
		EntityIdentityHdlr self;
		GravesAPI API;
};

#endif
