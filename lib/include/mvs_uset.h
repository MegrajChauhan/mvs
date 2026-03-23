#ifndef _MVS_USET_
#define _MVS_USET_

#include <mvs_results.h>
#include <mvs_tools.h>
#include <mvs_types.h>
#include <stdlib.h>

typedef struct MVSUSet MVSUSet;
typedef struct MVSUSetBucket MVSUSetBucket;

struct MVSUSetBucket {
  mptr_t key;
  MVSUSetBucket *nxt_bucket;
};

struct MVSUSet {
  MVSUSetBucket **buckets;
  size_t bucket_count;
  mhhfunc_t hash_func;
  mhkeycmpfunc_t cmp_func;
  mhcleanfunc_t key_clean_func; // cleanup function for the key
};

mResult_t mvs_uset_create(MVSUSet **uset, msize_t bucket_count,
                          mhhfunc_t hash_func, mhkeycmpfunc_t cmp_func,
                          mhcleanfunc_t kcf);

mResult_t mvs_uset_insert(MVSUSet *set, mptr_t key);

mResult_t mvs_uset_contains(MVSUSet *set, mptr_t key, mbool_t *res);

mResult_t mvs_uset_destroy(MVSUSet *set);

#endif
