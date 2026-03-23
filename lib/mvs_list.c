#include <mvs_list.h>

mResult_t mvs_static_list_create(msize_t cap, msize_t elem_len,
                                 MVSStaticList **lst) {
  if (!lst || cap == 0 || elem_len == 0)
    return MRES_INVALID_ARGS;
  *lst = (MVSStaticList *)malloc(sizeof(MVSStaticList));
  if (!(*lst)) {
    return MRES_SYS_FAILURE;
  }
  (*lst)->buf = (mptr_t)calloc(cap, elem_len);
  if (!(*lst)->buf) {
    free(*lst);
    return MRES_SYS_FAILURE;
  }
  (*lst)->cap = cap;
  (*lst)->curr_ind = 0;
  (*lst)->elem_len = elem_len;
  return MRES_SUCCESS;
}

mResult_t mvs_static_list_destroy(MVSStaticList *lst) {
  if (!lst)
    return MRES_INVALID_ARGS;
  free(lst->buf);
  free(lst);
  return MRES_SUCCESS;
}

mResult_t mvs_static_list_push(MVSStaticList *lst, mptr_t elem) {
  if (!lst || !elem)
    return MRES_INVALID_ARGS;

  if (lst->curr_ind >= lst->cap)
    return MRES_COULDNT_COMPLETE;

  memcpy((mptr_t)((mbptr_t)lst->buf + (lst->curr_ind * lst->elem_len)), elem,
         lst->elem_len);
  lst->curr_ind++;
  return MRES_SUCCESS;
}

mResult_t mvs_static_list_pop(MVSStaticList *lst, mptr_t elem) {
  if (!lst)
    return MRES_INVALID_ARGS;
  if (lst->curr_ind == 0)
    return MRES_COULDNT_COMPLETE;
  lst->curr_ind--;
  if (elem)
    memcpy(elem, (mptr_t)((mbptr_t)lst->buf + (lst->curr_ind * lst->elem_len)),
           lst->elem_len);
  return MRES_SUCCESS;
}

mResult_t mvs_static_list_at(MVSStaticList *lst, mptr_t elem, msize_t ind) {
  if (!lst || !elem || (ind >= lst->cap))
    return MRES_INVALID_ARGS;
  memcpy(elem, (mptr_t)((mbptr_t)lst->buf + (ind * lst->elem_len)),
         lst->elem_len);
  return MRES_SUCCESS;
}

mResult_t mvs_static_list_ref_of(MVSStaticList *lst, mptr_t elem, msize_t ind) {
  if (!lst || !elem || (ind >= lst->cap))
    return MRES_INVALID_ARGS;
  *(mbptr_t *)elem = (mbptr_t)lst->buf + (lst->elem_len * ind);
  return MRES_SUCCESS;
}

mResult_t mvs_static_list_resize(MVSStaticList *lst, msize_t resize_factor) {
  if (!lst || !resize_factor)
    return MRES_INVALID_ARGS;
  if (resize_factor == 1)
    return MRES_SUCCESS;
  mptr_t new_buf = (mptr_t)malloc(lst->elem_len * lst->cap * resize_factor);
  if (!new_buf)
    return MRES_SYS_FAILURE;
  mempcpy(new_buf, lst->buf, lst->curr_ind * lst->elem_len);
  mptr_t tmp = lst->buf;
  lst->buf = new_buf;
  lst->cap *= resize_factor;
  free(tmp);
  return MRES_SUCCESS;
}

_MVS_ATTR_ALWAYS_INLINE_ msize_t mvs_static_list_size(MVSStaticList *lst) {
  if (!lst)
    return (msize_t)-1;
  return lst->curr_ind;
}

_MVS_ATTR_ALWAYS_INLINE_ msize_t mvs_static_list_index_of(MVSStaticList *lst,
                                                          mptr_t elem) {
  if (!lst || !elem)
    return (msize_t)-1;
  return (msize_t)(((mbptr_t)elem - (mbptr_t)lst->buf) / lst->elem_len);
}

mResult_t mvs_lf_list_create(msize_t cap, msize_t elem_len, MVSLFList **lst) {
  if (!lst || cap == 0 || !elem_len)
    return MRES_INVALID_ARGS;
  *lst = (MVSLFList *)malloc(sizeof(MVSLFList));
  if (!(*lst)) {
    return MRES_SYS_FAILURE;
  }
  (*lst)->buf = (mptr_t)calloc(cap, elem_len);

  if (!(*lst)->buf) {
    free(lst);
    return MRES_SYS_FAILURE;
  }
  (*lst)->cap = cap;
  (*lst)->curr_ind = 0;
  (*lst)->elem_len = elem_len;
  return MRES_SUCCESS;
}

mResult_t mvs_lf_list_destroy(MVSLFList *lst) {
  if (!lst)
    return MRES_INVALID_ARGS;
  free(lst->buf);
  free(lst);
}

mResult_t mvs_lf_list_push(MVSLFList *lst, mptr_t elem) {
  if (!lst || !elem)
    return MRES_INVALID_ARGS;
  msize_t ind = atomic_fetch_add_explicit((_Atomic msize_t *)&lst->curr_ind, 1,
                                          memory_order_relaxed);
  if (ind >= lst->cap) {
    atomic_fetch_sub_explicit((_Atomic msize_t *)&lst->curr_ind, 1,
                              memory_order_relaxed);
    return MRES_COULDNT_COMPLETE;
  }
  memcpy((mptr_t)((mbptr_t)lst->buf + (ind * lst->elem_len)), elem,
         lst->elem_len);
  return MRES_SUCCESS;
}

mResult_t mvs_lf_list_pop(MVSLFList *lst, mptr_t elem) {
  if (!lst)
    return MRES_INVALID_ARGS;
  msize_t ind = atomic_fetch_sub_explicit((_Atomic msize_t *)curr_ind, 1,
                                          memory_order_relaxed);
  // since msize_t is just uint64_t so 0 - 1 would cause ind to be (msize_t)-1
  // i.e 2^64 or whatever it is called I forgot
  if (ind >= lst->cap) {
    atomic_fetch_sub_explicit((_Atomic msize_t *)&lst->curr_ind, 1,
                              memory_order_relaxed);
    return MRES_COULDNT_COMPLETE;
  }
  if (elem)
    memcpy(elem, (mptr_t)((mbptr_t)lst->buf + (ind * lst->elem_len)),
           lst->elem_len);
  return MRES_SUCCESS;
}

mResult_t mvs_dynamic_list_create(msize_t cap, msize_t elem_len,
                                  MVSDynamicList **lst) {
  if (cap == 0 || !elem_len || !lst)
    return MRES_INVALID_ARGS;
  *lst = (MVSDynamicList *)malloc(sizeof(MVSDynamicList));
  if (!(*lst)) {
    return MRES_SYS_FAILURE;
  }
  (*lst)->buf = (mptr_t)calloc(cap, elem_len);
  if (!(*lst)->buf) {
    free(*lst);
    return MRES_SYS_FAILURE;
  }
  (*lst)->cap = cap;
  (*lst)->curr_ind = 0;
  (*lst)->elem_len = elem_len;
  return MRES_SUCCESS;
}

mResult_t mvs_dynamic_list_destroy(MVSDynamicList *lst) {
  if (!lst)
    return MRES_INVALID_ARGS;
  free(lst->buf);
  free(lst);
}

mResult_t mvs_dynamic_list_push(MVSDynamicList *lst, mptr_t elem) {
  if (!lst || !elem)
    return MRES_INVALID_ARGS;
  if (lst->curr_ind >= lst->cap) {
    mResult_t res;
    if ((res = mvs_dynamic_list_resize(lst, 2)) != MRES_SUCCESS)
      return res;
  }
  memcpy((mptr_t)((mbptr_t)lst->buf + (lst->curr_ind * lst->elem_len)), elem,
         lst->elem_len);
  lst->curr_ind++;
  return MRES_SUCCESS;
}

mResult_t mvs_dynamic_list_pop(MVSDynamicList *lst, mptr_t elem) {
  if (!lst)
    return MRES_INVALID_ARGS;
  if (lst->curr_ind == 0)
    return MRES_COULDNT_COMPLETE;
  lst->curr_ind--;
  if (elem)
    memcpy(elem, (mptr_t)((mbptr_t)lst->buf + (lst->curr_ind * lst->elem_len)),
           lst->elem_len);
  return MRES_SUCCESS;
}

mResult_t mvs_dynamic_list_at(MVSDynamicList *lst, mptr_t elem, msize_t ind) {
  if (!lst || !elem || (ind >= lst->cap))
    return MRES_INVALID_ARGS;
  memcpy(elem, (mptr_t)((mbptr_t)lst->buf + (ind * lst->elem_len)),
         lst->elem_len);
  return MRES_SUCCESS;
}

mResult_t mvs_dynamic_list_ref_of(MVSDynamicList *lst, mptr_t *elem,
                                  msize_t ind) {
  if (!lst || !elem || (ind >= lst->cap))
    return MRES_INVALID_ARGS;
  *(mbptr_t *)elem = (mptr_t)((mbptr_t)lst->buf + (ind * lst->elem_len));
  return MRES_SUCCESS;
}

mResult_t mvs_dynamic_list_resize(MVSDynamicList *lst, msize_t resize_factor) {
  if (!lst)
    return MRES_INVALID_ARGS;
  mptr_t new_buf = (mptr_t)malloc(lst->elem_len * lst->cap * resize_factor);
  if (!new_buf)
    return MRES_SYS_FAILURE;
  mempcpy(new_buf, lst->buf, lst->curr_ind * lst->elem_len);
  mptr_t tmp = lst->buf;
  lst->buf = new_buf;
  lst->cap *= resize_factor;
  free(tmp);
  return MRES_SUCCESS;
}

_MVS_ATTR_ALWAYS_INLINE_ msize_t mvs_dynamic_list_size(MVSDynamicList *lst) {
  if (!lst)
    return (msize_t)(-1);
  return lst->curr_ind;
}

_MVS_ATTR_ALWAYS_INLINE_ msize_t mvs_dynamic_list_index_of(MVSDynamicList *lst,
                                                           mptr_t elem) {
  if (!lst || !elem)
    return 0;
  return (msize_t)(((mbptr_t)elem - (mbptr_t)lst->buf) / lst->elem_len);
}
