#include <merry_arg_utils.h>

_MVS_ATTR_INTERNAL_ msize_t merry_get_command_count(mstr_t str) {
   mstr_t iter = str;
   msize_t count = 0;
   while (*iter != '\0') {
		while (_MVS_MFUNC_ISSPACE_(*iter)) {
		    iter++;
		}
		while (!_MVS_MFUNC_ISSPACE_(*iter) && *iter != '\0') {
		    iter++;
		}
		count++;
   }
   return count;
}

mstr_t* merry_get_commands_from_string(mstr_t str) {
   msize_t count = merry_get_command_count(str);
   if (!count) {
		return NULL; // this shouldn't happen
   }
   mstr_t *args = (mstr_t*)malloc(sizeof(mstr_t) * count);
   if (!args) {
		MERRY_ERR("Memory allocation failed for SLIST commands parsing");
		return NULL;
   }
   mstr_t iter = str;
   msize_t i = 0;
   while (*iter != '\0') {
		while (_MVS_MFUNC_ISSPACE_(*iter))
		  iter++;
		msize_t len = 0;
		mstr_t st = iter;
		while (!_MVS_MFUNC_ISSPACE_(*iter) && *iter != '\0') {
		  iter++;
		}
		len = iter - st;
		mstr_t cmd = (mstr_t)malloc(len+1);
		if (!cmd) {
		   MERRY_ERR("Memory allocation failed for SLIST commands parsing");
		   goto __merry_get_commands_from_string_terminate;
		}
		memcpy(cmd, st, len);
		cmd[len] = 0;
		args[i] = cmd;
		i++;
   }
   return args;
__merry_get_commands_from_string_terminate:
   for (msize_t j = 0; j < i; j++) {
     free(args[j]);
   }
   free(args);
   return NULL;
}
