#include <api_entity.h>
#include <api_graves.h>
#include <api_results.h>
#include <mvs_types.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Mock Mock;

_MVS_ATTR_INTERNAL_ GravesAPI api;
struct Mock {
  EntityContext ctx;
  int NO_IDEA;
};

msize_t mock_create(EntityContext *ctx, mbptr_t *repr, msize_t conf) {
  Mock *m = (Mock *)malloc(sizeof(Mock));
  if (!m) {
    api.LOG_ERR("Mock initialization failed");
    return 1;
  }
  m->ctx = *ctx; // the passed ctx is a temporary variable, thus, the entity
                 // should copy it to itself
  m->NO_IDEA = 100;
  *repr = (mbptr_t)m;
  return 0;
}

msize_t mock_destroy(mptr_t repr) {
  free(repr);
  return 0;
}

msize_t mock_run(mptr_t repr) {
  /*
   * Tests performed:
   * */
  Mock *mock = (Mock*)repr;
  api.LOG_NOTE("MOCK is running");
  if (mock->ctx.slist) {
    api.LOG_NOTE("MOCK launched from a SLIST file");
	if (mock->ctx.argv)
		api.LOG_NOTE("Argument provided: %s", *(mock->ctx.argv));
	else
		api.LOG_NOTE("No arguments were provided");
  } else {
    api.LOG_NOTE("MOCK launched from command line");
	if (mock->ctx.argv) {
		api.LOG_NOTE("Provided %zu arguments", mock->ctx.argc);
		for (msize_t i = 0; i < mock->ctx.argc; i++) {
		  api.LOG_NOTE("Argument provided: %s", (mock->ctx.argv[i]));
		}
    } else
		api.LOG_NOTE("No arguments were provided");
  }
  return 0;
}

msize_t mock_get_default_setup() { return 0; }

mbool_t mock_check_setup(msize_t setup) { return mtrue; }

msize_t mock_deduce_setup(mstr_t key, mstr_t val, mbool_t *res) {
  api.LOG_NOTE("DEDUCE SETUP: %s=%s", key, val);
  *res = mtrue;
  return 0;
}

msize_t entity_register(msize_t EID, GravesAPI API) {
  printf("registering mock\n");
  api = API;
  EntityRegistryEntry entry = _API_MAKE_ENTITY_REGISTRY_ENTRY_(
      mock_create, mock_destroy, mock_run, mock_get_default_setup,
      mock_check_setup, mock_deduce_setup);
  printf("got %zu\n", api.register_component(EID, &entry)); // EID for mock is 0
  return 0;
}
