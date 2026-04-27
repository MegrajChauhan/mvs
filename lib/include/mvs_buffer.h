#ifndef _MVS_BUFFER_
#define _MVS_BUFFER_

/*
 * This is where multiple types of buffers will be defined. The rest of the
 * system(and modules) will use the buffers from here for storage. This module
 * will provide static and dynamic buffers. Static buffers are circular(for
 * proper use of space). Dynamic buffers are either linear or linked list. In
 * case multiple threads access the buffers, concurrency models implemented: 1)
 * SPSC 2) SPMC 3) MPSC 4) MPMC All of the buffers are going to be lock-free,
 * thus, dynamic resizing won't be allowed in multi-threaded cases. Due to
 * multiple threads accessing the buffers simultaneously in LF buffers, it
 * becomes extremely complex to resize and ensure every user has the correct and
 * new pointer.
 */

#include <mvs_results.h>
#include <mvs_tools.h>
#include <mvs_types.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>

/*
 * The reason that so many types are used below is because making a single
 * structure configurable to encompass all the above-mentioned models would be
 * complex and inefficient. Each model has its own behavior and constraints,
 * thus, separate structures and functions is a better choice.
 */
typedef struct MVSSimpleStaticBuffer
    MVSSimpleStaticBuffer; // for one owner to use(static)
typedef struct MVSSimpleDynamicBufferLinear
    MVSSimpleDynamicBufferLinear; // for one owner(uses linear array for
                                  // storage)
typedef struct MVSAllDynamicBufferLinkedListNode
    MVSAllDynamicBufferLinkedListNode;
typedef struct MVSSimpleDynamicBufferLinkedList
    MVSSimpleDynamicBufferLinkedList; // for one owner(uses linked lists for
                                      // storage)
typedef struct MVSHybridConcurrencyModelBuffer
    MVSHybridConcurrencyModelBuffer; // one single structure to encompass all of
                                     // the concurrency models at once

struct MVSAllDynamicBufferLinkedListNode {
  MVSAllDynamicBufferLinkedListNode *nxt, *prev;
  mbptr_t data; // the element
};

struct MVSSimpleStaticBuffer {
  mbptr_t buffer;
  msize_t buffer_capacity;
  msize_t elem_count;
  msize_t elem_len;
  msize_t head, tail;
};

#define _MVS_MFUNC_SIMPLE_STATIC_BUF_CHECK_ISFULL_(buf)                        \
  ((buf) && ((buf)->elem_count == (buf)->buffer_capacity))
#define _MVS_MFUNC_SIMPLE_STATIC_BUF_CHECK_ISEMPTY_(buf)                       \
  ((buf) && ((buf)->elem_count == 0))

mResult_t mvs_simple_static_buf_create(MVSSimpleStaticBuffer **buf, msize_t cap,
                                       msize_t elem_len);
mResult_t mvs_simple_static_buf_destroy(MVSSimpleStaticBuffer *buf);
// In read/write, store_in and elem should be pointers of data type stored so if
// 'int' is stored, than int*. If the stored data itself is a pointer, than a
// double pointer is used and so on.
mResult_t mvs_simple_static_buf_read(MVSSimpleStaticBuffer *buf,
                                     mptr_t store_in);
mResult_t mvs_simple_static_buf_write(MVSSimpleStaticBuffer *buf, mptr_t elem);
mResult_t mvs_simple_static_buf_flush(
    MVSSimpleStaticBuffer *buf); // WARNING: All data is deleted!

struct MVSSimpleDynamicBufferLinear {
  mbptr_t buffer;
  msize_t buffer_capacity;
  msize_t elem_count;
  msize_t elem_len;
  msize_t head, tail;
  msize_t resize_factor; // by default 2
};

#define _MVS_MFUNC_SIMPLE_DYNAMIC_LBUF_CHECK_ISFULL_(buf)                      \
  ((buf) && ((buf)->elem_count == (buf)->buffer_capacity))
#define _MVS_MFUNC_SIMPLE_DYNAMIC_LBUF_CHECK_ISEMPTY_(buf)                     \
  ((buf) && ((buf)->elem_count == 0))

mResult_t mvs_simple_dynamic_lbuf_create(MVSSimpleDynamicBufferLinear **buf,
                                         msize_t cap, msize_t elem_len);
mResult_t mvs_simple_dynamic_lbuf_destroy(MVSSimpleDynamicBufferLinear *buf);
mResult_t mvs_simple_dynamic_lbuf_read(MVSSimpleDynamicBufferLinear *buf,
                                       mptr_t store_in);
mResult_t mvs_simple_dynamic_lbuf_write(MVSSimpleDynamicBufferLinear *buf,
                                        mptr_t elem);
mResult_t mvs_simple_dynamic_lbuf_flush(MVSSimpleDynamicBufferLinear *buf);
mResult_t mvs_simple_dynamic_lbuf_resize(
    MVSSimpleDynamicBufferLinear *buf,
    msize_t
        factor); // Using this function will update "resize_factor" to "factor"

struct MVSSimpleDynamicBufferLinkedList {
  /*
   * To avoid repeated allocation and quicker insertion/deletion, the list keeps
   * track of previously read elements and stores them.
   */
  MVSAllDynamicBufferLinkedListNode *head, *tail;
  MVSAllDynamicBufferLinkedListNode *free_list_head, *free_list_tail;
  msize_t data_count;
  msize_t elem_len;
  msize_t free_list_node_count;
  msize_t free_list_node_limit; // 10 by default: These type of fields in lib
                                // can be configured using various flags
};

#define _MVS_MFUNC_SIMPLE_DYNAMIC_LLBUF_CHECK_ISEMPTY_(buf)                    \
  ((buf) && ((buf)->data_count == 0))

// MVSSimpleDynamicBufferLinkedList doesn't get resize because it dynamically
// adjusts to the data written using linked list

mResult_t
mvs_simple_dynamic_llbuf_create(MVSSimpleDynamicBufferLinkedList **buf,
                                msize_t elem_len);
mResult_t
mvs_simple_dynamic_llbuf_destroy(MVSSimpleDynamicBufferLinkedList *buf);
mResult_t mvs_simple_dynamic_llbuf_read(MVSSimpleDynamicBufferLinkedList *buf,
                                        mptr_t store_in);
mResult_t mvs_simple_dynamic_llbuf_write(MVSSimpleDynamicBufferLinkedList *buf,
                                         mptr_t elem);
mResult_t mvs_simple_dynamic_llbuf_flush(MVSSimpleDynamicBufferLinkedList *buf);

struct MVSHybridConcurrencyModelBuffer {
  mbptr_t buffer;
  atm_mbyte_t *slot_states; // state of each slot(1 = full else empty)
  msize_t buffer_capacity;
  msize_t elem_len;
  atm_msize_t elem_count; // not really dependable
  atm_msize_t head, tail;
  atm_msize_t owner_count;
};

#define _MVS_HYBRID_CONCURRENCY_MODEL_BUF_CHECK_ISFULL_(buf)                   \
  ((buf) &&                                                                    \
   (atomic_load_explicit(&(buf)->elem_count, memory_order_relaxed) ==       \
    (buf)->buffer_capacity))
#define _MVS_HYBRID_CONCURRENCY_MODEL_BUF_CHECK_ISEMPTY_(buf)                  \
  ((buf) &&                                                                    \
   (atomic_load_explicit(&(buf)->elem_count, memory_order_relaxed) == 0))

mResult_t
mvs_hybrid_concurrency_model_buf_create(MVSHybridConcurrencyModelBuffer **buf,
                                        msize_t cap, msize_t elem_len);
mResult_t
mvs_hybrid_concurrency_model_buf_destroy(MVSHybridConcurrencyModelBuffer *buf);
mResult_t
mvs_hybrid_concurrency_model_buf_read(MVSHybridConcurrencyModelBuffer *buf,
                                      mptr_t store_in);
mResult_t
mvs_hybrid_concurrency_model_buf_write(MVSHybridConcurrencyModelBuffer *buf,
                                       mptr_t elem);
// flush is something that would be too risky. Since multiple agents access this
// buffer, it would be unfair if one of them just decided to flush the entire
// buffer
mResult_t mvs_hybrid_concurrency_model_buf_add_owner(
    MVSHybridConcurrencyModelBuffer *buf);

#endif
