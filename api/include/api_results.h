#ifndef _API_RESULTS_
#define _API_RESULTS_

/*
 * The results of an API call.
 * The status of the API call, the source of any error, and the error code for that error
 * */

#include <mvs_types.h>

typedef enum apiRes_t apiRes_t;

enum apiRes_t {
		API_RES_SUCCESS,
		API_RES_FAILURE,
		API_RES_INVALID_ARGS,
		API_RES_REQ_NOT_SERVED, // The request hasn't been served yet
		API_RES_REQ_SERVED,     // The request has been served
		API_RES_INVALID_ARGS, // The request included invalid arguments
		API_RES_RETRY,
		API_RES_NO_MEM,
};

#endif
