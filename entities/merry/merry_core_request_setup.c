#include <merry_core_request_setup.h>

#define _MCREQSETUP_(req) merry_core_req_setup##req

typedef GravesRequest* (*mcreqsetup_t)(MerryCore *, MerryGravesRequest*);

mcreqsetup_t SETUP_FUNCS[] = {
	_MCREQSETUP_(something),
};

merry_core_req_setupREQ(something) {
   return NULL;
}
