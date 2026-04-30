#include <mvs_buffer.h>

mResult_t mvs_simple_static_buf_create(MVSSimpleStaticBuffer **buf, msize_t cap,
                                       msize_t elem_len) {
  if (!buf || !cap || !elem_len)
    return MRES_INVALID_ARGS;
  MVSSimpleStaticBuffer *b =
      (MVSSimpleStaticBuffer *)malloc(sizeof(MVSSimpleStaticBuffer));
  if (!b)
    return MRES_SYS_FAILURE;
  // The elem_len should be properly aligned for better performance
  // The buffer doesn't care about it though
  // That's the owner problem
  b->buffer = (mbptr_t)malloc(cap * elem_len);
  if (!b->buffer) {
    free(b);
    return MRES_SYS_FAILURE;
  }
  b->buffer_capacity = cap;
  b->elem_count = 0;
  b->elem_len = elem_len;
  b->head = 0; // Point to the next element to be popped
  b->tail = 0; // point to the next index where the new element is pushed
  *buf = b;
  return MRES_SUCCESS;
}

mResult_t mvs_simple_static_buf_destroy(MVSSimpleStaticBuffer *buf) {
  if (!buf)
    return MRES_SYS_FAILURE;
  if (buf->buffer)
    free(buf->buffer);
  free(buf);
  return MRES_SUCCESS;
}

mResult_t mvs_simple_static_buf_read(MVSSimpleStaticBuffer *buf,
                                     mptr_t store_in) {
  if (!buf || !store_in)
    return MRES_INVALID_ARGS;
  if (!buf->elem_count)
    return MRES_CONTAINER_EMPTY;
  memcpy(store_in,
         (mptr_t)(buf->buffer +
                  ((buf->head % buf->buffer_capacity) * buf->elem_len)),
         buf->elem_len);
  buf->head++;
  buf->elem_count--;
  return MRES_SUCCESS;
}

mResult_t mvs_simple_static_buf_write(MVSSimpleStaticBuffer *buf, mptr_t elem) {
  if (!buf || !elem)
    return MRES_INVALID_ARGS;
  if (buf->elem_count == buf->buffer_capacity)
    return MRES_CONTAINER_EMPTY;
  memcpy((mptr_t)(buf->buffer +
                  ((buf->tail % buf->buffer_capacity) * buf->elem_len)),
         elem, buf->elem_len);
  buf->tail++;
  buf->elem_count++;
  return MRES_SUCCESS;
}

mResult_t mvs_simple_static_buf_flush(MVSSimpleStaticBuffer *buf) {
  if (!buf)
    return MRES_INVALID_ARGS;
  buf->elem_count = 0;
  buf->head = 0;
  buf->tail = 0;
  return MRES_SUCCESS;
}

mResult_t mvs_simple_dynamic_lbuf_create(MVSSimpleDynamicBufferLinear **buf,
                                         msize_t cap, msize_t elem_len) {
  if (!buf || !cap || !elem_len)
    return MRES_INVALID_ARGS;
  MVSSimpleDynamicBufferLinear *b = (MVSSimpleDynamicBufferLinear *)malloc(
      sizeof(MVSSimpleDynamicBufferLinear));
  if (!b)
    return MRES_SYS_FAILURE;
  // the same alignment issue applies here too
  b->buffer = (mbptr_t)malloc(cap * elem_len);
  if (!b->buffer) {
    free(b);
    return MRES_SYS_FAILURE;
  }
  b->buffer_capacity = cap;
  b->elem_count = 0;
  b->elem_len = elem_len;
  b->head = 0; // Point to the next element to be popped
  b->tail = 0; // point to the next index where the new element is pushed
  *buf = b;
  return MRES_SUCCESS;
}

mResult_t mvs_simple_dynamic_lbuf_destroy(MVSSimpleDynamicBufferLinear *buf) {
  if (!buf)
    return MRES_SYS_FAILURE;
  if (buf->buffer)
    free(buf->buffer);
  free(buf);
  return MRES_SUCCESS;
}

mResult_t mvs_simple_dynamic_lbuf_read(MVSSimpleDynamicBufferLinear *buf,
                                       mptr_t store_in) {
  if (!buf || !store_in)
    return MRES_INVALID_ARGS;
  if (!buf->elem_count)
    return MRES_CONTAINER_EMPTY;
  memcpy(store_in,
         (mptr_t)(buf->buffer +
                  ((buf->head % buf->buffer_capacity) * buf->elem_len)),
         buf->elem_len);
  buf->head++;
  buf->elem_count--;
  return MRES_SUCCESS;
}

mResult_t mvs_simple_dynamic_lbuf_write(MVSSimpleDynamicBufferLinear *buf,
                                        mptr_t elem) {
  if (!buf || !elem)
    return MRES_INVALID_ARGS;
  if (buf->elem_count == buf->buffer_capacity) {
    mResult_t res;
    if ((res = mvs_simple_dynamic_lbuf_resize(
             buf, _MVS_CONSTANT_BUFFER_DEFAULT_RESIZE_FACTOR_)) != MRES_SUCCESS)
      return res;
  }
  memcpy((mptr_t)(buf->buffer +
                  ((buf->tail % buf->buffer_capacity) * buf->elem_len)),
         elem, buf->elem_len);
  buf->tail++;
  buf->elem_count++;
  return MRES_SUCCESS;
}

mResult_t mvs_simple_dynamic_lbuf_flush(MVSSimpleDynamicBufferLinear *buf) {
  if (!buf)
    return MRES_INVALID_ARGS;
  buf->elem_count = 0;
  buf->head = 0;
  buf->tail = 0;
  return MRES_SUCCESS;
}

mResult_t mvs_simple_dynamic_lbuf_resize(MVSSimpleDynamicBufferLinear *buf,
                                         msize_t factor) {
  if (!buf)
    return MRES_INVALID_ARGS;
  if (!factor)
    factor = _MVS_CONSTANT_BUFFER_DEFAULT_RESIZE_FACTOR_; // default value
  mbptr_t temp = (mbptr_t)malloc(buf->buffer_capacity * factor * buf->elem_len);
  if (!temp)
    return MRES_SYS_FAILURE;
  if (buf->elem_count) {
    for (msize_t i = 0; i < buf->elem_count; i++) {
      memcpy((mptr_t)(temp + i * buf->elem_len),
             (mptr_t)(buf->buffer +
                      (buf->head % buf->buffer_capacity) * buf->elem_len),
             buf->elem_len);
      buf->head++;
    }
  }
  buf->head = 0;
  buf->tail = buf->elem_count;
  buf->buffer_capacity *= factor;
  free(buf->buffer);
  buf->buffer = temp;
  return MRES_SUCCESS;
}

_MVS_ATTR_INTERNAL_ void mvs_simple_dynamic_llbuf_push_to_free_list(
    MVSSimpleDynamicBufferLinkedList *buf,
    MVSAllDynamicBufferLinkedListNode *node) {
  if (buf->free_list_node_count == buf->free_list_node_limit) {
    free(node); // cannot store the node
    return;
  }
  buf->free_list_tail->nxt = node;
  node->nxt = NULL;
  node->prev = buf->free_list_tail;
  buf->free_list_tail = node;
  buf->free_list_node_count++;
  return;
}

_MVS_ATTR_INTERNAL_ MVSAllDynamicBufferLinkedListNode *
mvs_simple_dynamic_llbuf_get_new_node(MVSSimpleDynamicBufferLinkedList *buf) {
  // if there is an available free node, return that else try to allocate a new
  // one
  MVSAllDynamicBufferLinkedListNode *res = NULL;
  if (buf->free_list_node_count) {
    res = buf->free_list_head;
    if (res->nxt)
      res->nxt->prev = NULL;
    buf->free_list_head = res->nxt;
    buf->free_list_node_count--;
    if (!buf->free_list_node_count) {
      buf->free_list_head = NULL;
      buf->free_list_tail = NULL;
    }
  } else {
    // The following math is this:
    // MVSAllDynamicBufferLinkedListNode has size X where size of nxt, prev, and
    // data are equal.
    //
    res = (MVSAllDynamicBufferLinkedListNode *)malloc(
        sizeof(MVSAllDynamicBufferLinkedListNode) + buf->elem_len);
    if (res) {
      res->data =
          (mptr_t)(&res->data +
                   sizeof(mbptr_t)); // We have a buffer after the 'data' field
                                     // that stores the data
    }
  }
  return res;
}

mResult_t
mvs_simple_dynamic_llbuf_create(MVSSimpleDynamicBufferLinkedList **buf,
                                msize_t elem_len) {
  if (!buf || !elem_len)
    return MRES_INVALID_ARGS;
  MVSSimpleDynamicBufferLinkedList *b =
      (MVSSimpleDynamicBufferLinkedList *)malloc(
          sizeof(MVSSimpleDynamicBufferLinkedList));
  if (!b)
    return MRES_SYS_FAILURE;
  b->head = NULL;
  b->tail = NULL;
  b->free_list_head = NULL;
  b->free_list_tail = NULL;
  b->data_count = 0;
  b->elem_len = elem_len;
  b->free_list_node_count = 0;
  b->free_list_node_limit = 10; // default
  *buf = b;
  return MRES_SUCCESS;
}

mResult_t
mvs_simple_dynamic_llbuf_destroy(MVSSimpleDynamicBufferLinkedList *buf) {
  if (!buf)
    return MRES_INVALID_ARGS;
  MVSAllDynamicBufferLinkedListNode *curr = buf->head;
  while (curr) {
    MVSAllDynamicBufferLinkedListNode *nxt = curr->nxt;
    free(curr);
    curr = nxt;
  }
  curr = buf->free_list_head;
  while (curr) {
    MVSAllDynamicBufferLinkedListNode *nxt = curr->nxt;
    free(curr);
    curr = nxt;
  }
  free(buf);
  return MRES_SUCCESS;
}

mResult_t mvs_simple_dynamic_llbuf_read(MVSSimpleDynamicBufferLinkedList *buf,
                                        mptr_t store_in) {
  if (!buf || !store_in)
    return MRES_INVALID_ARGS;
  // read from head
  // it becomes straightforward in linked lists for reading/writing
  if (!buf->data_count)
    return MRES_CONTAINER_EMPTY;
  MVSAllDynamicBufferLinkedListNode *data_node = buf->head;
  memcpy(store_in, data_node->data, buf->elem_len);
  buf->head = buf->head->nxt;
  mvs_simple_dynamic_llbuf_push_to_free_list(buf, data_node);
  buf->data_count--;
  if (!buf->data_count) {
    buf->head = NULL;
    buf->tail = NULL;
  }
  return MRES_SUCCESS;
}

mResult_t mvs_simple_dynamic_llbuf_write(MVSSimpleDynamicBufferLinkedList *buf,
                                         mptr_t elem) {
  if (!buf || !elem)
    return MRES_INVALID_ARGS;
  MVSAllDynamicBufferLinkedListNode *new_node =
      mvs_simple_dynamic_llbuf_get_new_node(buf);
  if (!new_node)
    return MRES_SYS_FAILURE;
  memcpy(new_node->data, elem, buf->elem_len);
  if (!buf->data_count) {
    new_node->nxt = NULL;
    new_node->prev = NULL;
    buf->head = new_node;
    buf->tail = new_node;
  } else {
    new_node->prev = buf->tail;
    buf->tail->nxt = new_node;
    buf->tail = new_node;
  }
  buf->data_count++;
  return MRES_SUCCESS;
}

mResult_t
mvs_simple_dynamic_llbuf_flush(MVSSimpleDynamicBufferLinkedList *buf) {
  // flushing in the llbuf will destroy all of the data
  if (!buf)
    return MRES_INVALID_ARGS;
  if (!buf->data_count)
    return MRES_SUCCESS;
  MVSAllDynamicBufferLinkedListNode *curr = buf->head;
  while (curr) {
    MVSAllDynamicBufferLinkedListNode *nxt = curr->nxt;
    mvs_simple_dynamic_llbuf_push_to_free_list(buf, curr);
    curr = nxt;
  }
  buf->data_count = 0;
  return MRES_SUCCESS;
}

mResult_t
mvs_hybrid_concurrency_model_buf_create(MVSHybridConcurrencyModelBuffer **buf,
                                        msize_t cap, msize_t elem_len) {
  if (!buf || !cap || !elem_len)
    return MRES_INVALID_ARGS;
  MVSHybridConcurrencyModelBuffer *b =
      (MVSHybridConcurrencyModelBuffer *)malloc(
          sizeof(MVSHybridConcurrencyModelBuffer));
  if (!b)
    return MRES_SYS_FAILURE;
  b->buffer = (mbptr_t)malloc(cap * elem_len);
  if (!b->buffer) {
    free(b);
    return MRES_SYS_FAILURE;
  }
  b->slot_states = (atm_mbyte_t *)malloc(sizeof(atm_mbyte_t) * cap);
  if (!b->slot_states) {
    free(b->buffer);
    free(b);
    return MRES_SYS_FAILURE;
  }
  b->buffer_capacity = cap;
  b->elem_len = elem_len;
  atomic_init(&b->head, 0); // Point to the next element to be popped
  atomic_init(&b->tail,
              0); // point to the next index where the new element is pushed
  atomic_init(&b->elem_count, 0);
  atomic_init(&b->owner_count, 0);
  for (msize_t i = 0; i < cap; i++) {
    atomic_init(&b->slot_states[i], 0);
  }
  *buf = b;
  return MRES_SUCCESS;
}

mResult_t
mvs_hybrid_concurrency_model_buf_destroy(MVSHybridConcurrencyModelBuffer *buf) {
  // This is not safe so the owners must coordinate to destroy the buffer only
  // after the buffer is no longer needed.
  if (!buf)
    return MRES_INVALID_ARGS;
  if (atomic_load_explicit(&buf->owner_count, memory_order_relaxed) > 1) {
    atomic_fetch_sub(&buf->owner_count, 1);
    return MRES_SUCCESS;
  }
  free(buf->slot_states);
  free(buf->buffer);
  free(buf);
  return MRES_SUCCESS;
}

mResult_t
mvs_hybrid_concurrency_model_buf_read(MVSHybridConcurrencyModelBuffer *buf,
                                      mptr_t store_in) {
  if (!buf || !store_in)
    return MRES_INVALID_ARGS;
  msize_t tries = 0;
  while (mtrue) {
    if (tries == buf->buffer_capacity)
      return MRES_CONTAINER_EMPTY;
    msize_t idx = atomic_load_explicit(&buf->head, memory_order_relaxed);
    msize_t slot = idx % buf->buffer_capacity;

    if (atomic_load_explicit(&buf->slot_states[slot], memory_order_acquire)) {
      if (atomic_compare_exchange_weak_explicit(
              &buf->head, &buf->head, buf->head + 1, memory_order_relaxed,
              memory_order_relaxed)) {
        memcpy(store_in, (mptr_t)(buf->buffer + slot * buf->elem_len),
               buf->elem_len);
        atomic_store_explicit(&buf->slot_states[slot], 0, memory_order_release);
        atomic_fetch_sub(&buf->elem_count, 1);
        return MRES_SUCCESS;
      }
      tries++;
    }
  }
  return MRES_COULDNT_COMPLETE;
}

mResult_t
mvs_hybrid_concurrency_model_buf_write(MVSHybridConcurrencyModelBuffer *buf,
                                       mptr_t elem) {
  if (!buf || !elem)
    return MRES_INVALID_ARGS;
  msize_t tries = 0;
  while (mtrue) {
    if (tries == buf->buffer_capacity)
      return MRES_CONTAINER_EMPTY;
    msize_t idx = atomic_load_explicit(&buf->tail, memory_order_relaxed);
    msize_t slot = idx % buf->buffer_capacity;

    if (!atomic_load_explicit(&buf->slot_states[slot], memory_order_acquire)) {
      if (atomic_compare_exchange_weak_explicit(
              &buf->tail, &buf->tail, buf->tail + 1, memory_order_relaxed,
              memory_order_relaxed)) {
        memcpy((mptr_t)(buf->buffer + slot * buf->elem_len), elem,
               buf->elem_len);
        atomic_store_explicit(&buf->slot_states[slot], 1, memory_order_release);
        atomic_fetch_add(&buf->elem_count, 1);
        return MRES_SUCCESS;
      }
      tries++;
    }
  }
  return MRES_COULDNT_COMPLETE;
}

mResult_t mvs_hybrid_concurrency_model_buf_add_owner(
    MVSHybridConcurrencyModelBuffer *buf) {
  if (!buf)
    return MRES_INVALID_ARGS;
  atomic_fetch_add(&buf->owner_count, 1);
  return MRES_SUCCESS;
}
