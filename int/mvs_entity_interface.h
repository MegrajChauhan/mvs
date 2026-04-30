#ifndef _MVS_ENTITY_INTERFACE_
#define _MVS_ENTITY_INTERFACE_

#include <mvs_types.h>
#include <mvs_config.h>
#include <mvs_entity_constants.h>

struct MVSEntityIdentity;
struct MVSGravesRequest;
struct MVSEntityIdentity;

typedef struct MVSEntityIdentity* MVSEntityIdentityHdlr;
typedef struct MVSEntityContext MVSEntityContext;
typedef struct MVSGravesAPI MVSGravesAPI;

/*
 * It is important to note that mResult_t is specifically for 'lib' and other modules that are build on top of it.
 * Graves and the components obviously have their own return types. 
 */
typedef msize_t (*mgravesmr_t)(MVSEntityIdentityHdlr, struct MVSGravesRequest*); // Graves Make Request -> make a request to Graves

struct MVSGravesAPI {
	mgravesmr_t make_request;
};

/*
 * This is provided to the Entity to use for accessing Graves
 */
struct MVSEntityContext {
	MVSEntityIdentityHdlr self;
	MVSGravesAPI API;
};

#endif
