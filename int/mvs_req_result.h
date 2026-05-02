#ifndef _MVS_REQ_RESULT_
#define _MVS_REQ_RESULT_

/*
 * These represent the primary return type.
 * These tell you the state of the request's execution
 */
#define _MVS_CONSTANT_REQUEST_REQ_SUCCESS_ 0
#define _MVS_CONSTANT_REQUEST_REQ_INVALID_ 1
#define _MVS_CONSTANT_REQUEST_REQ_TYPE_INVALID_ 2

/*
 * These represent the secondary return type.
 * These provide indepth look at the result of the request
 */
#define _MVS_CONSTANT_REQUEST_REQ_INVALID_EID_ 3
#define _MVS_CONSTANT_REQUEST_REQ_INVALID_CONFIG_ 4
#define _MVS_CONSTANT_REQUEST_REQ_INVALID_PROPERTY_ 5
#define _MVS_CONSTANT_REQUEST_REQ_ENTITY_NOT_REGISTERED_ 6
#define _MVS_CONSTANT_REQUEST_REQ_SYS_FAILURE_ 7
#define _MVS_CONSTANT_REQUEST_REQ_FAILED_LAUNCH_ 8

#endif
