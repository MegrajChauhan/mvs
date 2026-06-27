#ifndef _MERRY_CORE_REQUEST_SETUP_
#define _MERRY_CORE_REQUEST_SETUP_

#include <api_graves.h>
#include <merry_core.h>

#define merry_core_req_setupREQ(req)                                           \
  GravesRequest *merry_core_req_setup##req(MerryCore *, GravesAPI *)

merry_core_req_setupREQ(something);

#endif
