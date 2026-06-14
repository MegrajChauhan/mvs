#ifndef _API_GRAVES_
#define _API_GRAVES_

#include <api_results.h>
#include <api_types.h>
#include <api_entity.h>
#include <stdarg.h>
#include <api_request_response.h>
#include <mvs_protectors.h>
#include <mvs_types.h>

typedef struct EntityContext EntityContext;
typedef struct GravesAPI GravesAPI;

/*
 * The entity may use this API call to push a request.
 * The request may be synchronous or asynchronous based on if a wakeup condition
 * is provided or not. See the request API for more information. If the request
 * is synchronous, the call will not return until the request has been served
 * else, it will return immediately. If pushing the request fails, then also the
 * call will return immediately. The status of the API call will be returned as
 * apiRes_t
 * */
typedef apiRes_t (*gravesmr_t)(EntityIdentityHdlr, GravesRequest **);

/*
 * Graves Register component. The entity may use this to register itself
 * Return:
 * 0 = Success
 * 1 = Already Registered
 * 2 = Invalid entry
 */
typedef msize_t (*gravesrc_t)(msize_t ID, EntityRegistryEntry *entry);

typedef void (*graveslog_t)(mstr_t fmt, ...);

typedef void (*gravesvlog_t)(mstr_t fmt, va_list _l);

typedef apiRes_t (*gravesreqCS_t)(GravesRequest *);

typedef apiRes_t (*gravesreqGR_t)(GravesRequest *, APIRequestResponse *);

typedef apiRes_t (*gravesreqGRes_t)(GravesRequest *, GravesRequestResult *);

typedef apiRes_t (*gravesreqTypeSE_t)(EntityIdentityHdlr,
                                         msize_t, mqword_t,
                                         mqword_t, mqword_t,
                                         GravesRequest **);

struct GravesAPI {
  gravesmr_t make_request;
  gravesrc_t register_component;
  graveslog_t LOG;
  gravesvlog_t VLOG;
  gravesreqCS_t check_request_status;
  gravesreqGR_t get_request_response;
  gravesreqGRes_t get_request_result;
  gravesreqTypeSE_t make_request_SPAWN_ENTITY;
};

struct EntityContext {
  EntityIdentityHdlr self;
  mstr_t *argv;
  msize_t argc;
  mbool_t slist;
};

#endif
