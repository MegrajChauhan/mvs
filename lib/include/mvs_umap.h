#ifndef _MVS_UMAP_
#define _MVS_UMAP_

#include <mvs_results.h>
#include <mvs_tools.h>
#include <mvs_types.h>
#include <stdlib.h>

typedef struct MVSUmap MVSUmap;
typedef struct MVSUmapBucket MVSUmapBucket;

/*
 * MVSUmap and MVSUset are very inefficient in what they do.
 * The design may be improved in the future with optimization in mind but, for
 * now, this will work just fine The set of functions must be same just the
 * underlying logic has to change
 */

struct MVSUmapBucket {
  mptr_t value;
  mptr_t key;
  MVSUmapBucket *nxt_bucket;
};

struct MVSUmap {
  MVSUmapBucket **buckets;
  msize_t bucket_count;
  mhhfunc_t hash_func;
  mhkeycmpfunc_t cmp_func;
  mhcleanfunc_t key_clean_func;   // if the key needs to be cleaned
  mhcleanfunc_t value_clean_func; // if the value needs to be cleaned
};

mResult_t mvs_umap_create(MVSUmap **umap, msize_t bucket_count,
                          mhhfunc_t hash_func, mhkeycmpfunc_t kcmp,
                          mhcleanfunc_t kcf, mhcleanfunc_t vcf);

mResult_t mvs_umap_insert(MVSUmap *map, mptr_t key, mptr_t value);

mResult_t mvs_umap_find(MVSUmap *map, mptr_t key, mptr_t res);

mResult_t mvs_umap_destroy(MVSUmap *map);

#endif
