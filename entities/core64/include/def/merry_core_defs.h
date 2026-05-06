#ifndef _MERRY_CORE_DEFS_
#define _MERRY_CORE_DEFS_

#define _MERRY_CORE_PAGE_LEN_IN_BYTES_ 1048576 // 32KB
#define _MERRY_CORE_STACK_LEN_ 1048576       // 1MB
#define _MERRY_CORE_CALL_DEPTH_ 50

#define _MERRY_CORE_REQ_FLAG_ASYNC_REQUEST_ (1 << 0) 

/*
 * This represents the return values that core64 uses internally
 * */
enum {
  MERRY_RESULT_INVALID_SIR, // Invalid system interrupt request
  MERRY_RESULT_UNSUPPORTED_REQ, // The request isn't supported yet
  MERRY_RESULT_INVALID_MREQ, // Invalid MVS Request ID
};

#endif
