#ifndef _API_REQUEST_RESPONSE_
#define _API_REQUEST_RESPONSE_ 

#include <api_results.h>

#define _API_MAKE_REQUEST_RESPONSE_(status, src, resp) (APIRequestResponse){.request_fulfilled=(status), .source=(src), .response=(resp)}
#define _API_REQ_RESPONSE_GOOD_() _API_MAKE_REQUEST_RESPONSE_(mtrue, API_SRC_NONE, API_REQ_RESPONSE_NONE)
#define _API_REQ_RESPONSE_BAD_(src, resp) _API_MAKE_REQUEST_RESPONSE_(mfalse, src, resp)

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
		API_REQ_RESPONSE_ENTITY_LAUNCH_FAILED, // The launch of an entity failed
};

struct APIRequestResponse {
		mbool_t request_fulfilled; // Was the request successfully fulfilled?
        apiSrc_t source;           // If !request_fulfilled, what was the source of error?
		union {
				apiReqResponse_t response;
				msize_t code;
        };
};

#endif
