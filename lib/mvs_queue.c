#include <mvs_queue.h>

mResult_t mvs_llqueue_init(MVSLLQueue **queue) {
  if (!queue)
    return MRES_INVALID_ARGS;
  *queue = (MVSLLQueue *)malloc(sizeof(MVSLLQueue));
  if (!(*queue)) {
    return MRES_SYS_FAILURE;
  }
  (*queue)->head = (*queue)->tail = NULL;
  (*queue)->data_avai = 0;
  (*queue)->free_list = NULL;
  (*queue)->free_list_lim = 10;
  (*queue)->free_list_avai = 0;
  return MRES_SUCCESS;
}

mResult_t mvs_llqueue_push(MVSLLQueue *queue, mptr_t data) {
  if (!queue || !data)
    return MRES_INVALID_ARGS;
  MVSLLQueueNode *node;

  if (queue->free_list) {
    node = queue->free_list;
    queue->free_list = node->next_node;
    if (queue->free_list) {
      queue->free_list->prev_node = NULL;
    }
    queue->free_list_avai--;
  } else {
    node = (MVSLLQueueNode *)malloc(sizeof(MVSLLQueueNode));
    if (!node) {
      return MRES_SYS_FAILURE;
    }
  }
  node->data = *(mbptr_t *)data;
  if (queue->head == NULL && queue->tail == NULL) {
    node->next_node = NULL;
    node->prev_node = NULL;
    queue->head = queue->tail = node;
  } else {
    node->next_node = NULL;
    node->prev_node = queue->tail;
    queue->tail->next_node = node;
    queue->tail = node;
  }
  queue->data_avai++;
  return MRES_SUCCESS;
}

mResult_t mvs_llqueue_pop(MVSLLQueue *queue, mptr_t _store_in) {
  if (!queue)
    return MRES_INVALID_ARGS;
  if (queue->head == NULL && queue->tail == NULL) {
    return MRES_COULDNT_COMPLETE;
  }
  MVSLLQueueNode *head = queue->head;
  if (_store_in)
    *(mbptr_t *)_store_in = (mbptr_t)head->data;
  if (queue->head == queue->tail) {
    queue->tail = NULL;
    queue->head = NULL;
  } else {
    if (head->next_node)
      head->next_node->prev_node = NULL;
    queue->head = head->next_node;
  }
  if (queue->free_list_avai == queue->free_list_lim)
    free(head);
  else {
    // We just store the node instead
    if (queue->free_list->next_node)
      queue->free_list->next_node->prev_node = head;
    head->next_node = queue->free_list;
    queue->free_list = head;
    queue->free_list_avai++;
  }
  queue->data_avai--;
  return MRES_SUCCESS;
}

mResult_t mvs_llqueue_clear(MVSLLQueue *queue) {
  if (!queue)
    return MRES_INVALID_ARGS;
  if (queue->data_avai == 0)
    return MRES_SUCCESS;
  MVSLLQueueNode *curr = queue->head;
  while (curr != NULL) {
    MVSLLQueueNode *tmp = curr->next_node;
    free(curr);
    curr = tmp;
  }
  MVSLLQueueNode *curr = queue->free_list;
  while (curr != NULL) {
    MVSLLQueueNode *tmp = curr->next_node;
    free(curr);
    curr = tmp;
  }
  queue->head = NULL;
  queue->tail = NULL;
  queue->data_avai = 0;
  queue->free_list_avai = 0;
  return MRES_SUCCESS;
}

mResult_t mvs_llqueue_destroy(MVSLLQueue *queue) {
  if (!queue)
    return MRES_INVALID_ARGS;
  mvs_llqueue_clear(queue);
  free(queue);
}

mResult_t mvs_squeue_create(MVSSQueue **queue, msize_t cap, msize_t elen) {
  if ((!queue || cap == 0 || elen == 0))
    return MRES_INVALID_ARGS;
  *queue = (MVSSQueue *)malloc(sizeof(MVSSQueue));
  if (!(*queue)) {
    return MRES_SYS_FAILURE;
  }
  (*queue)->buf = malloc(cap * elen);
  if (!(*queue)->buf) {
    free(*queue);
    return MRES_SYS_FAILURE;
  }
  (*queue)->buf_cap = cap;
  (*queue)->elem_len = elen;
  (*queue)->head = (*queue)->rear = (mqword_t)(-1);
  return MRES_SUCCESS;
}

mResult_t mvs_squeue_top(MVSSQueue *queue, mptr_t elem) {
  if (!queue || !elem)
    return MRES_INVALID_ARGS;
  if (_MVS_MFUNC_STATIC_QUEUE_CHECK_EMPTY_(queue))
    return MRES_COULDNT_COMPLETE;
  *(mbptr_t *)elem = (((mbptr_t)queue->buf + (queue->head * queue->elem_len)));
  return MRES_SUCCESS;
}

mResult_t mvs_squeue_enqueue(MVSSQueue *queue, mptr_t elem) {
  if (!queue || !elem)
    return MRES_INVALID_ARGS;

  if (_MVS_MFUNC_STATIC_QUEUE_CHECK_FULL_(queue))
    return MRES_COULDNT_COMPLETE;

  if (_MVS_MFUNC_STATIC_QUEUE_CHECK_EMPTY_(queue))
    queue->head = 0;
  queue->rear = (queue->rear + 1) % queue->buf_cap;
  memcpy((mbptr_t)queue->buf + queue->rear * queue->elem_len, elem,
         queue->elem_len);
  return MRES_SUCCESS;
}

mResult_t mvs_squeue_dequeue(MVSSQueue *queue, mptr_t elem) {
  if (!queue)
    return MRES_INVALID_ARGS;

  if (_MVS_MFUNC_STATIC_QUEUE_CHECK_EMPTY_(queue))
    return MRES_COULDNT_COMPLETE;

  if (elem)
    memcpy(elem, (mptr_t)((mbptr_t)queue->buf + queue->head * queue->elem_len),
           queue->elem_len);

  if (queue->head == queue->rear)
    queue->head = queue->rear = (mqword_t)(-1);
  else
    queue->head = (queue->head + 1) % queue->buf_cap;

  return MRES_SUCCESS;
}

mResult_t mvs_squeue_destroy(MVSSQueue *queue) {
  if (!queue)
    return MRES_INVALID_ARGS;
  free(queue->buf);
  free(queue);
  return MRES_SUCCESS;
}

mResult_t mvs_squeue_atm_create(MVSSQueueAtm **queue, msize_t cap,
                                msize_t elen) {
  if ((!queue || cap == 0 || elen == 0))
    return MRES_INVALID_ARGS;
  *queue = (MVSSQueueAtm *)malloc(sizeof(MVSSQueueAtm));
  if (!(*queue)) {
    return MRES_SYS_FAILURE;
  }
  (*queue)->buf = malloc(cap * elen);
  if (!(*queue)->buf) {
    free(*queue);
    return MRES_SYS_FAILURE;
  }
  (*queue)->buf_cap = cap;
  (*queue)->elem_len = elen;
  (*queue)->head = (*queue)->rear = (mqword_t)(-1);
  return MRES_SUCCESS;
}

mResult_t mvs_squeue_atm_enqueue(MVSSQueueAtm *queue, mptr_t elem) {
  if (!queue || !elem)
    return MRES_INVALID_ARGS;

  msize_t id = atomic_fetch_add(&queue->rear, 1);
  msize_t inx = (id) % queue->buf_cap;

  if (_MVS_MFUNC_STATIC_QUEUE_ATM_CHECK_FULL_(queue, inx))
    return MRES_COULDNT_COMPLETE;

  memcpy((mbptr_t)queue->buf + inx * queue->elem_len, elem, queue->elem_len);
  return MRES_SUCCESS;
}

mResult_t mvs_squeue_atm_dequeue(MVSSQueueAtm *queue, mptr_t elem) {
  if (!queue)
    return MRES_INVALID_ARGS;

  if (_MVS_MFUNC_STATIC_QUEUE_ATM_CHECK_EMPTY_(queue, queue->rear))
    return MRES_COULDNT_COMPLETE;

  msize_t head = atomic_load_explicit(&queue->head, memory_order_relaxed);

  if (elem)
    memcpy(elem, (mptr_t)((char *)queue->buf + head * queue->elem_len),
           queue->elem_len);

  head = (head + 1) % queue->buf_cap;
  atomic_store_explicit(&queue->head, head, memory_order_release);

  return MRES_SUCCESS;
}

mResult_t mvs_squeue_atm_destroy(MVSSQueueAtm *queue) {
  if (!queue)
    return MRES_INVALID_ARGS;
  free(queue->buf);
  free(queue);
  return MRES_SUCCESS;
}
