#include <mvs_uset.h>

_MVS_ATTR_INTERNAL_ MVSUsetBucket *
mvs_uset_find_bucket(MVSUset *set, mptr_t key, msize_t *bucket_index) {
  *bucket_index = set->hash_func(key, set->bucket_count);
  MVSUsetBucket *bucket = set->buckets[*bucket_index];
  while (bucket && bucket->key) {
    if (set->cmp_func(bucket->key, key))
      return bucket;
    bucket = bucket->nxt_bucket;
  }
  return NULL;
}

mResult_t mvs_uset_create(MVSUset **uset, msize_t bucket_count,
                          mhhfunc_t hash_func, mhkeycmpfunc_t cmp_func,
                          mhcleanfunc_t kcf) {
  if (!uset || !hash_func || !kcf || !cmp_func || bucket_count == 0)
    return MRES_INVALID_ARGS;
  MVSUset *set = (MVSUset *)malloc(sizeof(MVSUset));
  if (!set)
    return MRES_SYS_FAILURE;

  set->buckets =
      (MVSUsetBucket **)calloc(bucket_count, sizeof(MVSUsetBucket *));
  if (!set->buckets) {
    free(set);
    return MRES_SYS_FAILURE;
  }
  set->bucket_count = bucket_count;
  set->hash_func = hash_func;
  set->cmp_func = cmp_func;
  set->key_clean_func = kcf;
  *uset = set;
  return MRES_SUCCESS;
}

mResult_t mvs_uset_insert(MVSUset *set, mptr_t key) {
  if (!set || !key)
    return MRES_INVALID_ARGS;
  msize_t bucket_index;
  MVSUsetBucket *bucket = mvs_uset_find_bucket(set, key, &bucket_index);

  if (bucket)
    return MRES_SUCCESS;

  MVSUsetBucket *new_bucket = (MVSUsetBucket *)malloc(sizeof(MVSUsetBucket));
  if (!new_bucket)
    return MRES_SYS_FAILURE;
  new_bucket->key = key;

  if (set->buckets[bucket_index]) {
    new_bucket->nxt_bucket = set->buckets[bucket_index];
    set->buckets[bucket_index] = new_bucket;
  } else {
    new_bucket->nxt_bucket = NULL;
    set->buckets[bucket_index] = new_bucket;
  }
  return MRES_SUCCESS;
}

mResult_t mvs_uset_contains(MVSUset *set, mptr_t key, mbool_t *res) {
  if (!set || !key || !res)
    return MRES_INVALID_ARGS;
  msize_t bucket_index;
  MVSUsetBucket *bucket = mvs_uset_find_bucket(set, key, &bucket_index);

  *res = (bucket != NULL) ? mtrue : mfalse;
  return MRES_SUCCESS;
}

mResult_t mvs_uset_destroy(MVSUset *set) {
  if (!set)
    return MRES_INVALID_ARGS;
  for (msize_t i = 0; i < set->bucket_count; i++) {
    MVSUsetBucket *bucket = set->buckets[i];
    while (bucket) {
      MVSUsetBucket *next = bucket->nxt_bucket;
      if (set->key_clean_func)
        set->key_clean_func(bucket->key);
      free(bucket);
      bucket = next;
    }
  }
  free(set->buckets);
  free(set);
  return MRES_SUCCESS;
}
