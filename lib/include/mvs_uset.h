#ifndef _MVS_USET_
#define _MVS_USET_

#include <mvs_results.h>
#include <mvs_tools.h>
#include <mvs_types.h>
#include <stdlib.h>

typedef struct MVSUset MVSUset;
typedef struct MVSUsetBucket MVSUsetBucket;

struct MVSUsetBucket {
  mptr_t key;
  MVSUsetBucket *nxt_bucket;
};

struct MVSUset {
  MVSUsetBucket **buckets;
  size_t bucket_count;
  mhhashfunc_t hash_func;
  mhkeycmpfunc_t cmp_func;
  mhcleanfunc_t key_clean_func; // cleanup function for the key
};

mResult_t mvs_uset_create(MVSUset **uset, msize_t bucket_count,
                          mhhashfunc_t hash_func, mhkeycmpfunc_t cmp_func,
                          mhcleanfunc_t kcf);

mResult_t mvs_uset_insert(MVSUset *set, mptr_t key);

mResult_t mvs_uset_contains(MVSUset *set, mptr_t key, mbool_t *res);

mResult_t mvs_uset_destroy(MVSUset *set);

#endif
