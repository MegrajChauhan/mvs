#ifndef _MVS_INT_RESULT_
#define _MVS_INT_RESULT_

#include <mvs_types.h>

#define _MVS_MFUNC_MAKE_INT_RESULT_(_status, _src, _code) {.source=(_status), .src=(_src), .code=(_code)}

typedef enum mIntResSrc_t mIntResSrc_t;
typedef struct MVSIntResult MVSIntResult;

enum mIntResSrc_t {
    MINT_SRC_LIB,    // In this case 'code' represents mResult_t 
	MINT_SRC_GRAVES, // In this case 'code' represents the return codes of Graves
	MINT_SRC_ENTITY, // In this case 'code' represents the return codes of the respective entity
	MINT_SRC_HOST,   // In this case 'code' represents the 'errno' value since the host failed us in some way
};

struct MVSIntResult {
		mbool_t status;
		mIntResSrc_t src;
		msize_t code;
};

#endif
