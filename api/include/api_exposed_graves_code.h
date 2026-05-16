#ifndef _API_EXPOSED_GRAVES_CODE_
#define _API_EXPOSED_GRAVES_CODE_

// Here we define the return codes that Graves will share with the API.
enum {
		API_CODE_REQ_NOT_SERVED, // The request hasn't been served yet
		API_CODE_REQ_SERVED,     // The request has been served
		API_CODE_REQ_INVALID_ARGS, // The request included invalid arguments
		API_CODE_REQ_COMP_FAILURE, // Component failure caused the request to fail
};

#endif
