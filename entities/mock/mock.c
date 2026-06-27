#include <api_entity.h>
#include <api_graves.h>
#include <api_results.h>
#include <mvs_types.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Mock Mock;

struct Mock {
  EntityContext ctx;
  int NO_IDEA;
};

msize_t mock_create(EntityContext *ctx, mbptr_t *repr, msize_t conf) {
  Mock *m = (Mock *)malloc(sizeof(Mock));
  if (!m) {
    ctx->API.LOG_CUSTOM(ctx->self, "Mock initialization failed");
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

msize_t mock_prepare(mptr_t repr) { return 0; }

msize_t mock_run(mptr_t repr) {
  /*
   * Tests performed:
   * */
  Mock *mock = (Mock *)repr;
  mock->ctx.API.LOG_CUSTOM(mock->ctx.self, "MOCK is running");
  if (mock->ctx.slist) {
    mock->ctx.API.LOG_CUSTOM(mock->ctx.self, "MOCK launched from a SLIST file");
    if (mock->ctx.argv)
      mock->ctx.API.LOG_CUSTOM(mock->ctx.self, "Argument provided: %s",
                               *(mock->ctx.argv));
    else
      mock->ctx.API.LOG_CUSTOM(mock->ctx.self, "No arguments were provided");
  } else {
    mock->ctx.API.LOG_CUSTOM(mock->ctx.self, "MOCK launched from command line");
    if (mock->ctx.argv) {
      mock->ctx.API.LOG_CUSTOM(mock->ctx.self, "Provided %zu arguments",
                               mock->ctx.argc);
      for (msize_t i = 0; i < mock->ctx.argc; i++) {
        mock->ctx.API.LOG_CUSTOM(mock->ctx.self, "Argument provided: %s",
                                 (mock->ctx.argv[i]));
      }
    } else
      mock->ctx.API.LOG_CUSTOM(mock->ctx.self, "No arguments were provided");
  }
  return 0;
}

msize_t mock_get_default_setup() { return 0; }

mbool_t mock_check_setup(msize_t setup) { return mtrue; }

msize_t mock_deduce_setup(mstr_t key, mstr_t val, mbool_t *res) {
  *res = mtrue;
  return 0;
}

EntityRegistryEntry entity_register(msize_t EID, GravesAPI API) {
  EntityRegistryEntry entry = _API_MAKE_ENTITY_REGISTRY_ENTRY_(
      mock_create, mock_destroy, mock_run, mock_get_default_setup,
      mock_check_setup, mock_deduce_setup, mock_prepare);
  return entry;
}
