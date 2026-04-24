#include <mvs_list.h>

mResult_t mvs_static_list_create(MVSStaticList **lst, msize_t cap,
                                 msize_t elem_len) {
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

mResult_t mvs_dynamic_listl_create(MVSDynamicListLinear **lst, msize_t cap,
                                   msize_t elem_len) {
  if (!lst || !cap || !elem_len)
    return MRES_INVALID_ARGS;
  *lst = (MVSDynamicListLinear *)malloc(sizeof(MVSDynamicListLinear));
  if (!(*lst)) {
    return MRES_SYS_FAILURE;
  }
  (*lst)->buf = (mptr_t)malloc(cap * elem_len);
  if (!(*lst)->buf) {
    free(*lst);
    return MRES_SYS_FAILURE;
  }
  (*lst)->cap = cap;
  (*lst)->curr_ind = 0;
  (*lst)->elem_len = elem_len;
  (*lst)->resize_factor = 2;
  return MRES_SUCCESS;
}

mResult_t mvs_dynamic_listl_destroy(MVSDynamicListLinear *lst) {
  if (!lst)
    return MRES_INVALID_ARGS;
  free(lst->buf);
  free(lst);
}

mResult_t mvs_dynamic_listl_push(MVSDynamicListLinear *lst, mptr_t elem) {
  if (!lst || !elem)
    return MRES_INVALID_ARGS;
  if (lst->curr_ind >= lst->cap) {
    mResult_t res;
    if ((res = mvs_dynamic_list_resize(lst, lst->resize_factor)) !=
        MRES_SUCCESS)
      return res;
  }
  memcpy((mptr_t)((mbptr_t)lst->buf + (lst->curr_ind * lst->elem_len)), elem,
         lst->elem_len);
  lst->curr_ind++;
  return MRES_SUCCESS;
}

mResult_t mvs_dynamic_listl_pop(MVSDynamicListLinear *lst, mptr_t elem) {
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

mResult_t mvs_dynamic_listl_size(MVSDynamicListLinear *lst, msize_t *res) {}

mResult_t mvs_dynamic_listl_resize(MVSDynamicListLinear *lst,
                                   msize_t resize_factor) {
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

_MVS_ATTR_INTERNAL_ void
mvs_dynamic_listll_push_to_free_list(MVSDynamicListLinkedList *lst,
                                     MVSDynamicListLinkedListNode *node) {
  if (lst->free_list_len == lst->free_list_limit) {
    free(node); // cannot store the node
    return;
  }
  node->next = lst->free_list;
  node->prev = NULL;
  lst->free_list->prev = node;
  lst->free_list = node;
  lst->free_list_len++;
  return;
}

_MVS_ATTR_INTERNAL_ MVSDynamicListLinkedListNode *
mvs_dynamic_listll_get_new_node(MVSDynamicListLinkedList *lst) {
  MVSDynamicListLinkedListNode *res = NULL;
  if (lst->free_list_len) {
    res = lst->free_list;
    if (res->next)
      res->next->prev = NULL;
    lst->free_list = res->next;
    lst->free_list_len--;
    if (!lst->free_list_len) {
      lst->free_list = NULL;
    }
  } else {
    res = (MVSDynamicListLinkedListNode *)malloc(
        sizeof(MVSDynamicListLinkedListNode) + lst->elem_len);
    if (res) {
      /// TODO: Maybe here it isn't 8 but 1
      // Since gcc incorporates data type length by default during address
      // calculations. &(mbptr_t)res->data becomes a pointer to a pointer i.e
      // has a size of 8-bytes. Which means +8 would add 8*8 instead which
      // wouldn't be ideal.
      res->data = (mptr_t)(&(mbptr_t)res->data +
                           8); // We have a buffer after the 'data' field that
                               // stores the data
    }
  }
  return res;
}

mResult_t mvs_dynamic_listll_create(MVSDynamicListLinkedList **lst,
                                    msize_t elem_len) {
  if (!lst || !elem_len)
    return MRES_INVALID_ARGS;
  MVSDynamicListLinkedList *l =
      (MVSDynamicListLinkedList *)malloc(sizeof(MVSDynamicListLinkedList));
  if (!l)
    return MRES_SYS_FAILURE;
  l->chain = NULL;
  l->free_list = NULL;
  l->data_count = 0;
  l->elem_len = elem_len;
  l->free_list_len = 0;
  l->free_list_limit = 10; // by default
  *lst = l;
  return MRES_SUCCESS;
}

mResult_t mvs_dynamic_listll_destroy(MVSDynamicListLinkedList *lst) {
  if (!lst)
    return MRES_INVALID_ARGS;
  MVSDynamicListLinkedListNode *curr = lst->head;
  while (curr) {
    MVSDynamicListLinkedListNode *tmp = curr->next;
    free(curr);
    curr = tmp;
  }
  curr = lst->free_list;
  while (curr) {
    MVSDynamicListLinkedListNode *tmp = curr->next;
    free(curr);
    curr = tmp;
  }
  free(lst);
  return MRES_SUCCESS;
}

mResult_t mvs_dynamic_listll_push(MVSDynamicListLinkedList *lst, mptr_t elem) {
  if (!lst || !elem)
    return MRES_INVALID_ARGS;
  MVSDynamicListLinkedListNode *new_node = mvs_dynamic_listll_get_new_node(buf);
  if (!new_node)
    return MRES_SYS_FAILURE;
  memcpy(new_node->data, elem, buf->elem_len);
  if (!lst->data_count) {
    new_node->next = NULL;
    new_node->prev = NULL;
    lst->chain = new_node;
    head = new_node;
  } else {
    new_node->prev = lst->chain;
    lst->chain->next = new_node;
    lst->chain = new_node;
  }
  buf->data_count++;
  return MRES_SUCCESS;
}

mResult_t mvs_dynamic_listll_pop(MVSDynamicListLinkedList *lst, mptr_t elem) {
  if (!lst || !elem)
    return MRES_INVALID_ARGS;
  if (!lst->data_count)
    return MRES_CONTAINER_EMPTY;
  MVSDynamicListLinkedListNode *data_node = lst->chain;
  memcpy(elem, data_node->data, lst->elem_len);
  lst->head = lst->head->next;
  mvs_dynamic_listll_push_to_free_list(lst, data_node);
  lst->data_count--;
  if (!lst->data_count) {
    buf->head = NULL;
    buf->chain = NULL;
  }
  return MRES_SUCCESS;
}
