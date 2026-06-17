#include <mvs_rlist.h>

mbool_t mvs_rlist_init(MVSRlist *rlist) {
  if (mvs_dynamic_listl_create(&rlist->entity_libs, 10,
                               sizeof(MVSDynamicLib *)) != MRES_SUCCESS)
    return mfalse;
  rlist->count = 0;
  return mtrue;
}

void mvs_rlist_destroy(MVSRlist *rlist) {
  if (rlist->count) {
    for (msize_t i = 0; i < rlist->count; i++) {
      mvs_dynamic_lib_destroy(
          *(MVSDynamicLib **)mvs_dynamic_listl_ref_of_unsafe(rlist->entity_libs,
                                                             i));
    }
  }
  mvs_dynamic_listl_destroy(rlist->entity_libs);
}

mbool_t mvs_rlist_load_from_list_file(MVSRlist *rlist, mstr_t file_path) {
  // At first, a number is expected to indicate how many entities are to be
  // loaded There are no comments to ignore, no whitespaces to ignore, only
  // commas and newlines.
  MVSRlistReader *r = mvs_rlist_reader_create();
  if (!r)
    return mfalse;
  if (!mvs_rlist_reader_init(r, file_path)) {
    mvs_rlist_reader_destroy(r);
    return mfalse;
  }
  while (mvs_rlist_reader_curr(r) == '\n')
    mvs_rlist_reader_consume(r);
  if (!_MVS_MFUNC_ISNUM_(mvs_rlist_reader_curr(r))) {
    fprintf(stderr, "[RLIST]: Expected the list length\n");
    return mfalse;
  }
  mstr_t st = mvs_rlist_reader_iter(r);
  while (_MVS_MFUNC_ISNUM_(mvs_rlist_reader_curr(r)))
    mvs_rlist_reader_consume(r);
  mstr_t ed = mvs_rlist_reader_iter(r);
  if (*ed != '\n') {
    fprintf(
        stderr,
        "[RLIST]: List length and the list must be separated by a newline\n");
    return mfalse;
  }
  msize_t diff = (msize_t)(ed - st);
  char num[diff + 1];
  num[diff] = 0;
  memcpy(num, st, diff);
  msize_t len = strtoull(num, NULL, 10);
  if (!len) {
    fprintf(stderr, "[RLIST]: List length cannot be zero\n");
    return mfalse;
  }
  mvs_rlist_reader_consume(r);
  msize_t iter = 0; // now this is going to act as the counter for how many
                    // entities were read
  while (mvs_rlist_reader_peek(r) != '\0') {
    while (mvs_rlist_reader_curr(r) == '\n')
      mvs_rlist_reader_consume(r);
    char curr = mvs_rlist_reader_curr(r);
    if (!_MVS_MFUNC_ISALPHA_(curr) && curr != '_') {
      fprintf(stderr, "[RLIST]: The list should only contain names\n");
      return mfalse;
    }
    st = mvs_rlist_reader_iter(r);
    while (_MVS_MFUNC_ISALNUM_(curr) || curr == '_') {
      mvs_rlist_reader_consume(r);
      curr = mvs_rlist_reader_curr(r);
    }
    ed = mvs_rlist_reader_iter(r);
    if (!mvs_rlist_reader_eof(r) && *ed != ',') {
      fprintf(stderr, "[RLIST]: Expected a separator ','\n");
      return mfalse;
    }
    diff = ed - st;
    char name[diff + 1 + 4 + 3];
    name[diff + 4 + 3] = 0;
    name[0] = 'o';
    name[1] = 'b';
    name[2] = 'j';
    name[3] = '/';
    name[diff + 4] = '.';
    name[diff + 4 + 1] = 's';
    name[diff + 4 + 2] = 'o';
    memcpy(name + 4, st, diff);
    /*
     * TODO: I guess 'lib' needs to provide a form of directory support too.
     * That way, one could open the directory(or whatever equivalent exists on
     * windows), check for the existence of anything and then make decisions.
     * For now, the assumption is that, anything MVS searches for exists, which
     * isn't a good assumption.
     * */
    MVSDynamicLib *lib;
    if (mvs_dynamic_lib_create(&lib) != MRES_SUCCESS) {
      fprintf(stderr, "[RLIST]: Failed to allocate memory for the entity\n");
      return mfalse;
    }
    /*
     * It is to be noted that nothing is stopping anyone from loading the same
     * entity twice. This is allowed because one may allocate one EID for one
     * specific configuration and later allocate another EID for the same entity
     * for another config. Thus, even though the underlying entity is the same,
     * the user may view them as different from a logical perspective. Of
     * course, the difference in config is visible only in the SLIST file.
     * */
    if (mvs_dynamic_lib_load_library(lib, name) != MRES_SUCCESS) {
      fprintf(stderr, "[RLIST]: Couldn't load the entity '%s'\n", name);
      mvs_dynamic_lib_destroy(lib);
      return mfalse;
    }
    if (mvs_dynamic_listl_push(rlist->entity_libs, &lib) != MRES_SUCCESS) {
      fprintf(stderr, "[RLIST]: Failed to register an entity\n");
      mvs_dynamic_lib_destroy(lib);
      return mfalse;
    }
    iter++;
  }
  if (iter != len) {
    fprintf(stderr, "[RLIST]: Expected %zu entries but got only %zu\n", len,
            iter);
    return mfalse;
  }
  rlist->count = len;
  mvs_rlist_reader_destroy(r);
  return mtrue;
}

mbool_t mvs_rlist_load_from_path(MVSRlist *rlist, mstr_t path) {
  MVSDynamicLib *lib;
  if (mvs_dynamic_lib_create(&lib) != MRES_SUCCESS) {
    mvs_log_err("[RLIST]: Failed to allocate memory for the entity\n");
    return mfalse;
  }
  if (mvs_dynamic_lib_load_library(lib, path) != MRES_SUCCESS) {
    mvs_log_err("[RLIST]: Couldn't load the entity '%s'\n", path);
    mvs_dynamic_lib_destroy(lib);
    return mfalse;
  }
  if (mvs_dynamic_listl_push(rlist->entity_libs, &lib) != MRES_SUCCESS) {
    mvs_log_err("[RLIST]: Failed to register an entity\n");
    mvs_dynamic_lib_destroy(lib);
    return mfalse;
  }
  rlist->count++;
  return mtrue;
}

mentityRegister_t mvs_rlist_get_registrar(MVSRlist *rlist, msize_t EID,
                                          mbool_t *sys) {
  *sys = mfalse;
  if (EID >= rlist->count)
    return NULL;
  mentityRegister_t func;
  if (mvs_dynamic_lib_get_symbol(
          *(MVSDynamicLib **)mvs_dynamic_listl_ref_of_unsafe(rlist->entity_libs,
                                                             EID),
          "entity_register", (mptr_t)&func) != MRES_SUCCESS) {
    *sys = mtrue;
    return NULL; // This only fails if the host fails since built-in entities
                 // are trusted
  }
  return func;
}
