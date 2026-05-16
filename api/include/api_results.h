#ifndef _API_RESULTS_
#define _API_RESULTS_

/*
 * The results of an API call.
 * The status of the API call, the source of any error, and the error code for that error
 * */

#include <mvs_types.h>

#define _API_MAKE_RESULT_(res, src, _code) (APIResult){.result=(res), .source=(src), .code=(_code)}
#define _API_SUCCESS_(src, code) _API_MAKE_RESULT_(API_RES_SUCCESS, src, code)
#define _API_FAILURE_(src, code) _API_MAKE_RESULT_(API_RES_FAILURE, src, code)
#define _API_BAD_CALL_(res) _API_MAKE_RESULT_(res, API_SRC_NONE, 0)
#define _API_GOOD_CALL_()   _API_SUCCESS_(API_SRC_NONE, 0)

typedef struct APIResult APIResult;
typedef enum apiRes_t apiRes_t;
typedef enum apiSrc_t apiSrc_t;

enum apiRes_t {
		API_RES_SUCCESS,
		API_RES_FAILURE,
		API_RES_INVALID_ARGS,
};

enum apiSrc_t {
		API_SRC_NONE,
		API_SRC_HOST,
		API_SRC_LIB,
		API_SRC_GRAVES,
		API_SRC_ENTITY,
};

struct APIResult {
		apiRes_t result;
		apiSrc_t source;
		msize_t code;
};

#endif
