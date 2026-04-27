#ifndef _MVS_QUEUE_
#define _MVS_QUEUE_

#include <mvs_buffer.h>

/*
 * Available queue types:
 * Static Queue(ring-queuefer)
 * Dynamic Queue(ring-queuefer and linked list)
 * Multi-threaded queues(A generic queue that can be used for SPSC, SPMC, MPSC,
 * MPMC)
 */

typedef MVSSimpleStaticBuffer MVSStaticQueue;
typedef MVSSimpleDynamicBufferLinear MVSDynamicQueueLinear;
typedef MVSSimpleDynamicBufferLinkedList MVSDynamicQueueLinkedList;
typedef MVSHybridConcurrencyModelBuffer MVSHybridConcurrencyModelQueue;

#define _MVS_MFUNC_STATIC_QUEUE_CHECK_ISFULL_(queue)                           \
  _MVS_MFUNC_SIMPLE_STATIC_BUF_CHECK_ISFULL_(queue)
#define _MVS_MFUNC_STATIC_QUEUE_CHECK_ISEMPTY_(queue)                          \
  _MVS_MFUNC_SIMPLE_STATIC_BUF_CHECK_ISEMPTY_(queue)

_MVS_ATTR_ALWAYS_INLINE_ mResult_t
mvs_static_queue_create(MVSStaticQueue **queue, msize_t cap, msize_t elem_len) {
  return mvs_simple_static_buf_create(queue, cap, elem_len);
}

_MVS_ATTR_ALWAYS_INLINE_ mResult_t
mvs_static_queue_destroy(MVSStaticQueue *queue) {
  return mvs_simple_static_buf_destroy(queue);
}

_MVS_ATTR_ALWAYS_INLINE_ mResult_t
mvs_static_queue_dequeue(MVSStaticQueue *queue, mptr_t store_in) {
  return mvs_simple_static_buf_read(queue, store_in);
}

_MVS_ATTR_ALWAYS_INLINE_ mResult_t
mvs_static_queue_enqueue(MVSStaticQueue *queue, mptr_t elem) {
  return mvs_simple_static_buf_write(queue, elem);
}

_MVS_ATTR_ALWAYS_INLINE_ mResult_t
mvs_static_queue_flush(MVSStaticQueue *queue) {
  return mvs_simple_static_buf_flush(queue);
}

#define _MVS_MFUNC_DYNAMIC_LQUEUE_CHECK_ISFULL_(queue)                         \
  _MVS_MFUNC_SIMPLE_DYNAMIC_LBUF_CHECK_ISFULL_(queue)
#define _MVS_MFUNC_DYNAMIC_LQUEUE_CHECK_ISEMPTY_(queue)                        \
  _MVS_MFUNC_SIMPLE_DYNAMIC_LBUF_CHECK_ISEMPTY_(queue)

_MVS_ATTR_ALWAYS_INLINE_
    mResult_t mvs_dynamic_lqueue_create(MVSDynamicQueueLinear **queue,
                                        msize_t cap, msize_t elem_len) {
  return mvs_simple_dynamic_lbuf_create(queue, cap, elem_len);
}

_MVS_ATTR_ALWAYS_INLINE_ mResult_t
mvs_dynamic_lqueue_destroy(MVSDynamicQueueLinear *queue) {
  return mvs_simple_dynamic_lbuf_destroy(queue);
}

_MVS_ATTR_ALWAYS_INLINE_ mResult_t
mvs_dynamic_lqueue_dequeue(MVSDynamicQueueLinear *queue, mptr_t store_in) {
  return mvs_simple_dynamic_lbuf_read(queue, store_in);
}

_MVS_ATTR_ALWAYS_INLINE_ mResult_t
mvs_dynamic_lqueue_enqueue(MVSDynamicQueueLinear *queue, mptr_t elem) {
  return mvs_simple_dynamic_lbuf_write(queue, elem);
}

_MVS_ATTR_ALWAYS_INLINE_ mResult_t
mvs_dynamic_lqueue_flush(MVSDynamicQueueLinear *queue) {
  return mvs_simple_dynamic_lbuf_flush(queue);
}

_MVS_ATTR_ALWAYS_INLINE_ mResult_t
mvs_dynamic_lqueue_resize(MVSDynamicQueueLinear *queue, msize_t factor) {
  return mvs_simple_dynamic_lbuf_resize(queue, factor);
}

#define _MVS_MFUNC_DYNAMIC_LLQUEUE_CHECK_ISEMPTY_(queue)                       \
  _MVS_MFUNC_SIMPLE_DYNAMIC_LLBUF_CHECK_ISEMPTY_(queue)

_MVS_ATTR_ALWAYS_INLINE_
    mResult_t mvs_dynamic_llqueue_create(MVSDynamicQueueLinkedList **queue,
                                         msize_t elem_len) {
  return mvs_simple_dynamic_llbuf_create(queue, elem_len);
}

_MVS_ATTR_ALWAYS_INLINE_ mResult_t
mvs_dynamic_llqueue_destroy(MVSDynamicQueueLinkedList *queue) {
  return mvs_simple_dynamic_llbuf_destroy(queue);
}

_MVS_ATTR_ALWAYS_INLINE_ mResult_t
mvs_dynamic_llqueue_dequeue(MVSDynamicQueueLinkedList *queue, mptr_t store_in) {
  return mvs_simple_dynamic_llbuf_read(queue, store_in);
}

_MVS_ATTR_ALWAYS_INLINE_ mResult_t
mvs_dynamic_llqueue_enqueue(MVSDynamicQueueLinkedList *queue, mptr_t elem) {
  return mvs_simple_dynamic_llbuf_write(queue, elem);
}

_MVS_ATTR_ALWAYS_INLINE_ mResult_t
mvs_dynamic_llqueue_flush(MVSDynamicQueueLinkedList *queue) {
  return mvs_simple_dynamic_llbuf_flush(queue);
}

#define _MVS_HYBRID_CONCURRENCY_MODEL_QUEUE_CHECK_ISFULL_(queue)               \
  _MVS_HYBRID_CONCURRENCY_MODEL_BUF_CHECK_ISFULL_(queue)
#define _MVS_HYBRID_CONCURRENCY_MODEL_QUEUE_CHECK_ISEMPTY_(queue)              \
  _MVS_HYBRID_CONCURRENCY_MODEL_BUF_CHECK_ISEMPTY_(queue)

_MVS_ATTR_ALWAYS_INLINE_ mResult_t mvs_hybrid_concurrency_model_queue_create(
    MVSHybridConcurrencyModelQueue **queue, msize_t cap, msize_t elem_len) {
  return mvs_hybrid_concurrency_model_buf_create(queue, cap, elem_len);
}

_MVS_ATTR_ALWAYS_INLINE_ mResult_t mvs_hybrid_concurrency_model_queue_destroy(
    MVSHybridConcurrencyModelQueue *queue) {
  return mvs_hybrid_concurrency_model_buf_destroy(queue);
}

_MVS_ATTR_ALWAYS_INLINE_ mResult_t mvs_hybrid_concurrency_model_queue_dequeue(
    MVSHybridConcurrencyModelQueue *queue, mptr_t store_in) {
  return mvs_hybrid_concurrency_model_buf_read(queue, store_in);
}

_MVS_ATTR_ALWAYS_INLINE_ mResult_t mvs_hybrid_concurrency_model_queue_enqueue(
    MVSHybridConcurrencyModelQueue *queue, mptr_t elem) {
  return mvs_hybrid_concurrency_model_buf_write(queue, elem);
}

_MVS_ATTR_ALWAYS_INLINE_ mResult_t mvs_hybrid_concurrency_model_queue_add_owner(
    MVSHybridConcurrencyModelQueue *queue) {
  return mvs_hybrid_concurrency_model_buf_add_owner(queue);
}

#endif
