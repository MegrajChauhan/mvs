#ifndef _MERRY_CORE_REQUEST_SETUP_
#define _MERRY_CORE_REQUEST_SETUP_

#include <merry_core.h>
#include <merry_requests.h>

#define merry_core_req_setupREQ(req) mbool_t merry_core_req_setup##req(MerryCore *, MerryGravesRequest *)

mbool_t merry_core_handle_async_requests(MerryCore *core);

mbool_t merry_core_handle_sync_requests(MerryCore *core);

merry_core_req_setupREQ(create_core);
merry_core_req_setupREQ(create_group);
merry_core_req_setupREQ(get_system_details);

#endif
