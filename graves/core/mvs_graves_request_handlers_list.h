#ifndef _MVS_GRAVES_REQUEST_HANDLER_LIST_
#define _MVS_GRAVES_REQUEST_HANDLER_LIST_

#include <mvs_graves_request_handlers.h>

typedef msize_t (*mgravesreqhdlr_t)(MVSGraves *, MVSGravesRequest *);

mgravesreqhdlr_t request_hdlrs[] = {
    mvs_request_handle_SPAWN_ENTITY,
};

#endif
