#include <mvs_umap.h>

_MVS_ATTR_INTERNAL_ MVSUmapBucket *
mvs_umap_find_bucket(MVSUmap *map, mptr_t key, msize_t *bucket_index) {
  *bucket_index = map->hash_func(key, map->bucket_count);
  MVSUmapBucket *bucket = map->buckets[*bucket_index];

  while (bucket && bucket->key) {
    if (map->cmp_func(bucket->key, key)) {
      return bucket;
    }
    bucket = bucket->nxt_bucket;
  }

  return NULL;
}

mResult_t mvs_umap_create(MVSUmap **umap, msize_t bucket_count,
                          mhhashfunc_t hash_func, mhkeycmpfunc_t kcmp,
                          mhcleanfunc_t kcf, mhcleanfunc_t vcf) {
  if (!umap || !hash_func || !kcf || !vcf || !kcmp || bucket_count == 0)
    return MRES_INVALID_ARGS;
  MVSUmap *map = (MVSUmap *)malloc(sizeof(MVSUmap));
  if (!map)
    return MRES_SYS_FAILURE;
  map->buckets =
      (MVSUmapBucket **)calloc(bucket_count, sizeof(MVSUmapBucket *));
  if (!map->buckets) {
    free(map);
    return MRES_SYS_FAILURE;
  }
  map->bucket_count = bucket_count;
  map->hash_func = hash_func;
  map->cmp_func = kcmp;
  map->key_clean_func = kcf;
  map->value_clean_func = vcf;
  *umap = map;
  return MRES_SUCCESS;
}

mResult_t mvs_umap_insert(MVSUmap *map, mptr_t key, mptr_t value) {
  if (!map || !key || !value)
    return MRES_INVALID_ARGS;
  msize_t bucket_index;
  MVSUmapBucket *bucket = mvs_umap_find_bucket(map, key, &bucket_index);
  if (bucket) {
    // key already exists, update value
    bucket->value = value;
    return MRES_SUCCESS;
  }

  MVSUmapBucket *new_bucket = (MVSUmapBucket *)malloc(sizeof(MVSUmapBucket));
  if (!new_bucket)
    return MRES_SYS_FAILURE;
  new_bucket->key = key;
  new_bucket->value = value;

  // insert new key-value pair
  if (map->buckets[bucket_index]) {
    new_bucket->nxt_bucket = map->buckets[bucket_index];
    map->buckets[bucket_index] = new_bucket;
  } else {
    new_bucket->nxt_bucket = NULL;
    map->buckets[bucket_index] = new_bucket;
  }
  return MRES_SUCCESS;
}

// Find Value by Key
mResult_t mvs_umap_find(MVSUmap *map, mptr_t key, mptr_t res) {
  if (!map || !key || !res)
    return MRES_INVALID_ARGS;
  msize_t bucket_index;
  MVSUmapBucket *bucket = mvs_umap_find_bucket(map, key, &bucket_index);
  *(mbptr_t*)res = bucket ? bucket->value : NULL;
  return MRES_SUCCESS;
}

mResult_t mvs_umap_destroy(MVSUmap *map) {
  if (!map)
    return MRES_INVALID_ARGS;
  for (msize_t i = 0; i < map->bucket_count; i++) {
    MVSUmapBucket *bucket = map->buckets[i];
    while (bucket) {
      MVSUmapBucket *next = bucket->nxt_bucket;
      if (map->key_clean_func)
        map->key_clean_func(bucket->key);
      if (map->value_clean_func)
        map->value_clean_func(bucket->value);
      free(bucket);
      bucket = next;
    }
  }
  map->buckets = NULL;
  free(map->buckets);
  free(map);
  return MRES_SUCCESS;
}
