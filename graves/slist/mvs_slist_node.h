#ifndef _MVS_SLIST_NODE_
#define _MVS_SLIST_NODE_

#include <mvs_slist_defs.h>

typedef struct MVSSlistNode MVSSlistNode;

struct MVSSlistNode {
		mSlistCommand_t command;
		union {
				struct {
						MVSStrSlice name;
						msize_t EID;
				} make_entity;
				struct {
						MVSStrSlice name;
						MVSStrSlice *value;
						msize_t value_list_len;
				} make_constant;
				struct {
						MVSStrSlice name;
						MVSStrSlice *value;
						msize_t value_list_len;
				} make_config;
				struct {
						MVSStrSlice name;
						msize_t
				} set_instance_count;
		};
};

#endif
