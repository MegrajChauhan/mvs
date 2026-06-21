#ifndef _MERRY_CORE_REQUEST_SETUP_
#define _MERRY_CORE_REQUEST_SETUP_

#include <merry_core.h>
#include <api_graves.h>

#define merry_core_req_setupREQ(req) GravesRequest* merry_core_req_setup##req(MerryCore *, GravesAPI *api)

merry_core_req_setupREQ(something);

#endif
