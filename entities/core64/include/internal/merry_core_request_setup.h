#ifndef _MERRY_CORE_REQUEST_SETUP_
#define _MERRY_CORE_REQUEST_SETUP_

#include <merry_core.h>
#include <merry_requests.h>
#include <mvs_entity_interface.h>
#include <mvs_request_interface.h>

#define merry_core_req_setupREQ(req) struct MVSGravesRequest* merry_core_req_setup##req(MerryCore *c, mcond_t *cond)

//mbool_t merry_core_handle_async_requests(MerryCore *core);

mbool_t merry_core_handle_sync_requests(MerryCore *core);

merry_core_req_setupREQ(spawn_entity);

#endif
