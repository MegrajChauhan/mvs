#ifndef _API_REQUEST_RESPONSE_
#define _API_REQUEST_RESPONSE_ 

#include <api_results.h>

#define _API_MAKE_REQUEST_RESPONSE_(status, src) (APIRequestResponse){.request_fulfilled=(status), .response=(resp)}
#define _API_REQ_RESPONSE_GOOD_() _API_MAKE_REQUEST_RESPONSE_(mtrue, API_REQ_RESPONSE_NONE)
#define _API_REQ_RESPONSE_BAD_(resp) _API_MAKE_REQUEST_RESPONSE_(mfalse, resp)

typedef struct APIRequestResponse APIRequestResponse;
typedef enum apiReqResponse_t apiReqResponse_t;

enum apiReqResponse_t {
		API_REQ_RESPONSE_NONE,
		API_REQ_RESPONSE_NOT_PERMITTED, // The request isn't allowed or some operation required by the request isn't allowed
		API_REQ_RESPONSE_INVALID_EID,   // Invalid EID provided for the request
		API_REQ_RESPONSE_INVALID_REQ,   // Invalid REQ ID provided
		API_REQ_RESPONSE_INVALID_CRED,  // Invalid credentials(identity) provided
		API_REQ_RESPONSE_UNREGISTERED_EID, // The EID given is valid but unregistered
		API_REQ_RESPONSE_INVALID_CONFIG,  // The configuration provided is invalid
		API_REQ_RESPONSE_INVALID_PROPERTY, // The property provided is invalid
		API_REQ_RESPONSE_INVALID_SETUP, // The provided setup for the entity launch is invalid
		API_REQ_RESPONSE_ENTITY_LAUNCH_FAILED, // The launch of an entity failed
		API_REQ_RESPONSE_ENTITY_INIT_FAILED,   // The entity failed to initialize
		API_REQ_RESPONSE_INVALID_DATA, // The data provided for some request was invalid
		API_REQ_RESPONSE_RESOURCE_EXHAUSTED, // The resource used by the request has been exhausted
		API_REQ_RESPONSE_RESOURCE_INVALID_STATE, // The resource used by the request is in an invalid state such that either any operation is impossible or just this operation is impossible
        API_REQ_RESPONSE_RETRY,  // Retry with the request later
		API_REQ_RESPONSE_HOST_FAILURE, // The host failed us not MVS
};

struct APIRequestResponse {
		mbool_t request_fulfilled; // Was the request successfully fulfilled?
		apiReqResponse_t response;
};

#endif
