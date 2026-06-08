#include <api_entity.h>
#include <api_graves.h>
#include <api_results.h>
#include <mvs_types.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct Mock Mock;

_MVS_ATTR_INTERNAL_ GravesAPI *api;
struct Mock {
		EntityContext ctx;
		int NO_IDEA;
};

msize_t mock_create(EntityContext *ctx, mbptr_t *repr, msize_t conf) {
   Mock *m = (Mock*)malloc(sizeof(Mock));
   if (!m) {
		   api->LOG("Mock initialization failed");
		   return 1;
   }
   m->ctx = *ctx; // the passed ctx is a temporary variable, thus, the entity should copy it to itself
   m->NO_IDEA = 100;
   *repr = (mbptr_t)m;
   return 0;
}

msize_t mock_destroy(mptr_t repr) {
		free(repr);
		return 0;
}

msize_t mock_run(mptr_t repr) {
		// temporarily, mock will just ask for input and print it
		_MVS_ATTR_LOCAL_ msize_t count = 0;
		int n;
		api->LOG("[Mock]: Enter a number: ");
		scanf("%d", &n);
		api->LOG("[MOCK]: You entered %d", n);
		count++;
		if (count % 10 == 0)
				return 0;
		return 1;
}

msize_t mock_get_default_setup() {
		return 0;
}

mbool_t mock_check_setup(msize_t setup) {
		return mtrue;
}

msize_t mock_deduce_setup(mstr_t key, mstr_t val, mbool_t *res) {
		*res = mtrue;
		return 0;
}


void entity_register(msize_t EID, GravesAPI *API) {
		printf("registering mock\n");
		api = API;
		EntityRegistryEntry entry = _API_MAKE_ENTITY_REGISTRY_ENTRY_(mock_create, mock_destroy,mock_run, mock_get_default_setup, mock_check_setup, mock_deduce_setup);
	    printf("got %zu\n", api->register_component(EID, &entry)); // EID for mock is 0	
}

