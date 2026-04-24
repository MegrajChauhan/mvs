#ifndef _MVS_LIST_
#define _MVS_LIST_

/*
 * Available list types:
 * 1. Static list
 * 2. lock free static list here
 * 3. Dynamic list(linear and linked list)
 * */

#include <mvs_logger.h>
#include <mvs_results.h>
#include <mvs_tools.h>
#include <mvs_types.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h> // memcpy

typedef struct MVSStaticList MVSStaticList;
typedef struct MVSDynamicListLinear MVSDynamicListLinear;
typedef struct MVSDynamicListLinkedListNode MVSDynamicListLinkedListNode;
typedef struct MVSDynamicListLinkedList MVSDynamicListLinkedList;
typedef struct MVSHybridConcurrencyModelList MVSHybridConcurrencyModelList;

struct MVSStaticList {
  msize_t cap;
  msize_t curr_ind;
  msize_t elem_len;
  mptr_t buf;
};

#define _MVS_MFUNC_STATIC_LIST_CHECK_ISFULL_(lst)                              \
  ((lst) && ((lst)->curr_ind == (lst)->cap))
#define _MVS_MFUNC_STATIC_LIST_CHECK_ISEMPTY_(lst)                             \
  ((lst) && ((lst)->curr_ind == 0))

mResult_t mvs_static_list_create(MVSStaticList **lst, msize_t cap,
                                 msize_t elem_len);
mResult_t mvs_static_list_destroy(MVSStaticList *lst);
mResult_t mvs_static_list_push(MVSStaticList *lst, mptr_t elem);
mResult_t mvs_static_list_pop(MVSStaticList *lst, mptr_t elem);
mResult_t mvs_static_list_resize(MVSStaticList *lst, msize_t resize_factor);

_MVS_ATTR_ALWAYS_INLINE_ mResult_t mvs_static_list_size(MVSStaticList *lst,
                                                        msize_t *res) {
  if (!lst || !res)
    return MRES_INVALID_ARGS;
  *res = lst->curr_ind;
  return MRES_SUCCESS;
}

_MVS_ATTR_ALWAYS_INLINE_ mResult_t mvs_static_list_ref_of(MVSStaticList *lst,
                                                          mptr_t elem,
                                                          msize_t ind) {
  if (!lst || !elem || (ind >= lst->curr_ind))
    return MRES_INVALID_ARGS;
  *(mbptr_t *)elem = (mbptr_t)lst->buf + ind * lst->elem_len;
  return MRES_SUCCESS;
}

_MVS_ATTR_ALWAYS_INLINE_ mResult_t mvs_static_list_index_of(MVSStaticList *lst,
                                                            mptr_t elem,
                                                            msize_t *idx) {
  if (!lst || !elem || !idx)
    return MRES_INVALID_ARGS;
  msize_t i = (msize_t)(((mbptr_t)elem - (mbptr_t)lst->buf) / lst->elem_len);
  if (i >= lst->cap)
    return MRES_INVALID_ARGS;
  *idx = i;
  return MRES_SUCCESS;
}

_MVS_ATTR_ALWAYS_INLINE_ mResult_t mvs_static_list_at(MVSStaticList *lst,
                                                      mptr_t elem,
                                                      msize_t ind) {
  if (!lst || !elem || (ind >= lst->cap))
    return MRES_INVALID_ARGS;
  memcpy(elem, (mptr_t)((mbptr_t)lst->buf + (ind * lst->elem_len)),
         lst->elem_len);
  return MRES_SUCCESS;
}

struct MVSDynamicListLinear {
  msize_t cap;
  msize_t curr_ind;
  msize_t elem_len;
  msize_t resize_factor; // by default: 2
  mptr_t buf;
};

#define _MVS_MFUNC_DYNAMIC_LISTL_CHECK_ISFULL_(lst)                            \
  ((lst) && ((lst)->curr_ind == (lst)->cap))
#define _MVS_MFUNC_DYNAMIC_LISTL_CHECK_ISEMPTY_(lst)                           \
  ((lst) && ((lst)->curr_ind == 0))

mResult_t mvs_dynamic_listl_create(MVSDynamicListLinear **lst, msize_t cap,
                                   msize_t elem_len);
mResult_t mvs_dynamic_listl_destroy(MVSDynamicListLinear *lst);
mResult_t mvs_dynamic_listl_push(MVSDynamicListLinear *lst, mptr_t elem);
mResult_t mvs_dynamic_listl_pop(MVSDynamicListLinear *lst, mptr_t elem);
mResult_t mvs_dynamic_listl_resize(MVSDynamicListLinear *lst,
                                   msize_t resize_factor);

_MVS_ATTR_ALWAYS_INLINE_ mResult_t
mvs_dynamic_listl_size(MVSDynamicListLinear *lst, msize_t *res) {
  if (!lst || !res)
    return MRES_INVALID_ARGS;
  *res = lst->curr_ind;
  return MRES_SUCCESS;
}

_MVS_ATTR_ALWAYS_INLINE_ mResult_t
mvs_dynamic_listl_ref_of(MVSDynamicListLinear *lst, mptr_t elem, msize_t ind) {
  if (!lst || !elem || (ind >= lst->cap))
    return MRES_INVALID_ARGS;
  *(mbptr_t *)elem = (mptr_t)((mbptr_t)lst->buf + (ind * lst->elem_len));
  return MRES_SUCCESS;
}

_MVS_ATTR_ALWAYS_INLINE_ mResult_t
mvs_dynamic_listl_at(MVSDynamicListLinear *lst, mptr_t elem, msize_t ind) {
  if (!lst || !elem || (ind >= lst->cap))
    return MRES_INVALID_ARGS;
  memcpy(elem, (mptr_t)((mbptr_t)lst->buf + (ind * lst->elem_len)),
         lst->elem_len);
  return MRES_SUCCESS;
}

struct MVSDynamicListLinkedListNode {
  MVSDynamicListLinkedListNode *next, *prev;
  mptr_t data;
};

struct MVSDynamicListLinkedList {
  MVSDynamicListLinkedListNode *chain, *head;
  MVSDynamicListLinkedListNode *free_list;
  msize_t data_count;
  msize_t elem_len;
  msize_t free_list_len;
  msize_t free_list_limit; // 10 by default
};

#define _MVS_MFUNC_DYNAMIC_LISTLL_CHECK_ISEMPTY_(lst) ((lst) && !(lst)->chain)

mResult_t mvs_dynamic_listll_create(MVSDynamicListLinkedList **lst,
                                    msize_t elem_len);
mResult_t mvs_dynamic_listll_destroy(MVSDynamicListLinkedList *lst);
mResult_t mvs_dynamic_listll_push(MVSDynamicListLinkedList *lst, mptr_t elem);
mResult_t mvs_dynamic_listll_pop(MVSDynamicListLinkedList *lst, mptr_t elem);

_MVS_ATTR_ALWAYS_INLINE_ mResult_t
mvs_dynamic_listll_size(MVSDynamicListLinkedList *lst, msize_t *res) {
  if (!lst || !res)
    return MRES_INVALID_ARGS;
  *res = lst->data_count;
  return MRES_SUCCESS;
}

/*
 * Lock-free lists are a nightmare to implement. Just use synchronization
 * primitives and the above basic structures.
 */

#endif
