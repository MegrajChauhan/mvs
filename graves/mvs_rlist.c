#include <mvs_rlist.h>

typedef void (*mentityRegister_t)(msize_t, GravesAPI *);

mbool_t mvs_rlist_init(MVSRlist *rlist) {
  MVSRlistReader *r = mvs_rlist_reader_create();
  if (!r)
    return mfalse;
  if (!mvs_rlist_reader_init(r, "./entities.l")) {
    mvs_rlist_reader_destroy(r);
	return mfalse;
  }
  rlist->count = 0;
  rlist->reader = r;
  return mtrue;
}

void mvs_rlist_destroy(MVSRlist *rlist) {
  mvs_rlist_reader_destroy(rlist->reader);
  if (rlist->entity_libs) {
    for (msize_t i = 0; i < rlist->count; i++) {
		mvs_dynamic_lib_destroy(rlist->entity_libs[i]);
	}
	free(rlist->entity_libs);
  }
}

mbool_t mvs_rlist_read(MVSRlist *rlist, GravesAPI *API) {
   // At first, a number is expected to indicate how many entities are to be loaded
   // There are no comments to ignore, no whitespaces to ignore, only commas and newlines.
   while (mvs_rlist_reader_curr(rlist->reader) == '\n') 
		   mvs_rlist_reader_consume(rlist->reader);
   if (!_MVS_MFUNC_ISNUM_(mvs_rlist_reader_curr(rlist->reader))) {
     fprintf(stderr, "[RLIST]: Expected the list length\n");
	 return mfalse;
   }
   mstr_t st = mvs_rlist_reader_iter(rlist->reader);
   while (_MVS_MFUNC_ISNUM_(mvs_rlist_reader_curr(rlist->reader))) 
		   mvs_rlist_reader_consume(rlist->reader);
   mstr_t ed = mvs_rlist_reader_iter(rlist->reader);
   if (*ed != '\n') {
      fprintf(stderr, "[RLIST]: List length and the list must be separated by a newline\n");
	  return mfalse;
   }
   msize_t diff = (msize_t)(ed - st);
   char num[diff+1];
   num[diff] = 0;
   memcpy(num, st, diff);
   msize_t len = strtoull(num, NULL, 10);
   if (!len) {
     fprintf(stderr, "[RLIST]: List length cannot be zero\n");
	 return mfalse;
   }
   rlist->entity_libs = (MVSDynamicLib**)malloc(sizeof(MVSDynamicLib*) * len);
   if (!rlist->entity_libs) {
		fprintf(stderr, "[RLIST]: Failed to allocate memory for entity list\n");
		return mfalse;
   }
   mvs_rlist_reader_consume(rlist->reader);
   msize_t iter = 0; // now this is going to act as the counter for how many entities were read
   while (mvs_rlist_reader_peek(rlist->reader) != '\0') {
	   while (mvs_rlist_reader_curr(rlist->reader) == '\n') 
		   mvs_rlist_reader_consume(rlist->reader);
	   char curr = mvs_rlist_reader_curr(rlist->reader);
 	   if (!_MVS_MFUNC_ISALPHA_(curr) && curr != '_') {
		 fprintf(stderr, "[RLIST]: The list should only contain names\n");
		 return mfalse;
	   }
	   st = mvs_rlist_reader_iter(rlist->reader);
	   while (_MVS_MFUNC_ISALNUM_(curr) || curr == '_') {
		 mvs_rlist_reader_consume(rlist->reader);
		 curr = mvs_rlist_reader_curr(rlist->reader);
	   }
	   ed = mvs_rlist_reader_iter(rlist->reader);
	   if (!mvs_rlist_reader_eof(rlist->reader) && *ed != ',') {
		 fprintf(stderr, "[RLIST]: Expected a separator ','\n");
		 return mfalse;
	   }
	   diff = ed - st;
	   char name[diff+1+4+3];
	   name[diff+4+3] = 0;
	   name[0] = 'o';
	   name[1] = 'b';
	   name[2] = 'j';
	   name[3] = '/';
	   name[diff+4] = '.';
	   name[diff+4+1] = 's';
	   name[diff+4+2] = 'o';
	   memcpy(name+4, st, diff);
	   /*
		* TODO: I guess 'lib' needs to provide a form of directory support too.
		* That way, one could open the directory(or whatever equivalent exists on windows),
		* check for the existence of anything and then make decisions. For now, the assumption
		* is that, anything MVS searches for exists, which isn't a good assumption.
		* */
	   MVSDynamicLib *lib;
	   if (mvs_dynamic_lib_create(&lib) != MRES_SUCCESS) {
		fprintf(stderr, "[RLIST]: Failed to allocate memory for the entity\n");
		return mfalse;
	   }
	   /*
		* It is to be noted that nothing is stopping anyone from loading the same entity twice.
		* This is allowed because one may allocate one EID for one specific configuration 
		* and later allocate another EID for the same entity for another config. Thus, even
		* though the underlying entity is the same, the user may view them as different
		* from a logical perspective. Of course, the difference in config is visible only in
		* the SLIST file.
		* */
	   if (mvs_dynamic_lib_load_library(lib, name) != MRES_SUCCESS) {
		fprintf(stderr, "[RLIST]: Couldn't load the entity '%s'\n", name);
		mvs_dynamic_lib_destroy(lib);
		return mfalse;
	   }
	   // Now each entity must provide the same function named "entity_register"
       mentityRegister_t func;
	   if (mvs_dynamic_lib_get_symbol(lib, "entity_register", (mptr_t)&func) != MRES_SUCCESS) {
		fprintf(stderr, "[RLIST]: Couldn't find 'entity_register' in %s\n", name);
		mvs_dynamic_lib_destroy(lib);
		return mfalse;
	   }
	   /*
		* Once the entity registers itself, there is really no need to access the library.
		* The needed functions are registered and MVS concerns itself with only those.
		* Those functions might call other functions internally and that will load the 
		* symbols automatically as requested(since we are lazy loading).
		* */
	   func(iter, API); // this should register the entity(iter is the EID)
       rlist->entity_libs[iter] = lib;
	   iter++;
   }
   if (iter != len) {
		fprintf(stderr, "[RLIST]: Expected %zu entries but got only %zu\n", len, iter);
		return mfalse;
   }
   rlist->count = len;
   return mtrue;
}
