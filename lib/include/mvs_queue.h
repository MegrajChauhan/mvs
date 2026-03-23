#ifndef _MVS_QUEUE_
#define _MVS_QUEUE_

#include <mvs_results.h>
#include <mvs_tools.h>
#include <mvs_types.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>

/*
 * We will need:
 * 1. Dynamic Queue using Linked Lists
 * 2. Static Queue
 * 3. Lock free static queue
 * Lock free Dynamic queue will be extremely hard
 * */

/*----------DYNAMIC QUEUE------------*/
/*
 * The Dynamic Queue only stores pointers although 8-byte numbers could be
 * stored. That would require using MVSPtrToQword or something similar for the
 * conversion
 */
typedef struct MVSLLQueueNode MVSLLQueueNode;
typedef struct MVSLLQueue MVSLLQueue;
struct MVSLLQueueNode {
  MVSLLQueueNode *next_node, *prev_node;
  mptr_t data;
};

struct MVSLLQueue {
  MVSLLQueueNode *head, *tail;
  MVSLLQueueNode *free_list;
  msize_t data_avai;
  msize_t free_list_lim;
  msize_t free_list_avai;
  /*
   * Here the free_list acts exactly what it sounds like- a list of free nodes
   * Constant allocating and freeing of nodes would be too inefficient but then
   * not freeing at all would also be too devastating, thus, free_list_lim is
   * used to limit how much nodes can be stored. That should be a configurable
   * option though it is hardcoded to be 10 at the moment
   */
};

mResult_t mvs_llqueue_init(MVSLLQueue **queue);
/*
 * Here 'data' should be a pointer to a pointer. Eg:
 * int *tmp = malloc(400);
 * mvs_llqueue_push(queue, &tmp);
 * Thus the queue will store the pointer itself
 */
mResult_t mvs_llqueue_push(MVSLLQueue *queue, mptr_t data);
/*
 * Here, _store_in must be a pointer to a pointer. As the queue stores pointers
 * directly, it will also return the pointer directly
 */
mResult_t mvs_llqueue_pop(MVSLLQueue *queue, mptr_t _store_in);
mResult_t mvs_llqueue_clear(MVSLLQueue *queue);
mResult_t mvs_llqueue_destroy(MVSLLQueue *queue);

/*----------END DYNAMIC QUEUE------------*/

/*----------STATIC QUEUE------------*/
typedef struct MVSSQueue MVSSQueue;

struct MVSSQueue {
  msize_t head, rear;
  mptr_t buf;
  msize_t buf_cap;
  msize_t elem_len;
};

#define _MVS_MFUNC_STATIC_QUEUE_CHECK_EMPTY_(queue)                            \
  ((queue)->head == (mqword_t)(-1))
#define _MVS_MFUNC_STATIC_QUEUE_CHECK_FULL_(queue)
((((queue)->rear + 1) % (queue)->buf_cap) == (queue)->head)
#define _MVS_MFUNC_STATIC_QUEUE_CLEAR_(queue)
    ((queue)->head = (queue)->rear = (mqword_t)(-1))

        mResult_t mvs_squeue_create(MVSSQueue **queue, msize_t cap,
                                    msize_t elen);

/*
 * This doesn't remove the element but returns a pointer to it.
 * Thus elem must be a pointer to a pointer
 */
mResult_t mvs_squeue_top(MVSSQueue *queue, mptr_t elem);

mResult_t mvs_squeue_enqueue(MVSSQueue *queue, mptr_t elem);

mResult_t mvs_squeue_dequeue(MVSSQueue *queue, mptr_t elem);

mResult_t mvs_squeue_destroy(MVSSQueue *queue);

/*----------END STATIC QUEUE------------*/

/*----------LF STATIC QUEUE------------*/
/*
 * This queue shares the same property of the MVSLFList which is that there
 * should only be one consumer while there could be multiple producers
 */
typedef struct MVSSQueueAtm MVSSQueueAtm;

struct MVSSQueueAtm {
  _Atomic msize_t head, rear;
  mptr_t buf;
  msize_t buf_cap;
  msize_t elem_len;
};

#define _MVS_MFUNC_STATIC_QUEUE_ATM_CHECK_EMPTY_(queue, idx)                   \
  ((queue)->head == (idx))
#define _MVS_MFUNC_STATIC_QUEUE_ATM_CHECK_FULL_(queue, idx)
(((idx + 1) % (queue)->buf_cap) == (queue)->head)
#define _MVS_MFUNC_STATIC_QUEUE_ATM_CLEAR_(queue)
    ((queue)->head = (queue)->rear = (mqword_t)(-1))

        mResult_t merry_squeue_atm_create(MVSSQueueAtm **queue, msize_t cap,
                                          msize_t elen);

mResult_t mvs_squeue_atm_enqueue(MVSSQueueAtm *queue, mptr_t elem);

mResult_t mvs_squeue_atm_dequeue(MVSSQueueAtm *queue, mptr_t elem);

mResult_t mvs_squeue_atm_destroy(MVSSQueueAtm *queue);

/*----------END LF STATIC QUEUE------------*/

#endif
