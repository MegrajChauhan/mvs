#include <mvs_stack.h>

mResult_t mvs_stack_init(MVSStack **stack, msize_t cap, msize_t elem_len) {
  if (!stack || !cap || !elem_len)
    return MRES_INVALID_ARGS;
  *stack = (MVSStack *)malloc(sizeof(MVSStack));
  if (!(*stack)) {
    return MRES_SYS_FAILURE;
  }
  (*stack)->buf = (mptr_t)malloc(elem_len * cap);
  if (!(*stack)->buf) {
    free(*stack);
    return MRES_SYS_FAILURE;
  }
  (*stack)->sp = (msize_t)(-1);
  (*stack)->cap = cap;
  (*stack)->sp_max = cap - 1;
  (*stack)->external_buffer = mfalse;
  (*stack)->elem_len = elem_len;
  return MRES_SUCCESS;
}

mResult_t mvs_stack_init_using_external_buf(MVSStack **stack, mptr_t buf,
                                            msize_t len, msize_t elem_len) {
  if (!stack || !buf || !elem_len || !len)
    return MRES_INVALID_ARGS;
  *stack = (MVSStack *)malloc(sizeof(MVSStack));
  if (!(*stack)) {
    return MRES_SYS_FAILURE;
  }
  (*stack)->buf = buf;
  (*stack)->sp = (msize_t)(-1);
  (*stack)->cap = len;
  (*stack)->sp_max = len - 1;
  (*stack)->external_buffer = mtrue;
  (*stack)->elem_len = elem_len;
  return MRES_SUCCESS;
}

mResult_t mvs_stack_push(MVSStack *stack, mptr_t value) {
  if (!stack || !value)
    return MRES_INVALID_ARGS;
  if (_MVS_MFUNC_STACK_CHECK_FULL_(stack))
    return MRES_COULDNT_COMPLETE;
  stack->sp++;
  memcpy((mbptr_t)stack->buf + stack->sp * stack->elem_len, value,
         stack->elem_len);
  return MRES_SUCCESS;
}

mResult_t mvs_stack_pop(MVSStack *stack, mptr_t elem) {
  if (!stack)
    return MRES_INVALID_ARGS;
  if (_MVS_MFUNC_STACK_CHECK_EMPTY_(stack))
    return MRES_COULDNT_COMPLETE;
  memcpy(elem, (mbptr_t)stack->buf + stack->sp * stack->elem_len,
         stack->elem_len);
  stack->sp--;
  return MRES_SUCCESS;
}

mResult_t mvs_stack_top(MVSStack *stack, mptr_t elem) {
  if (!stack || !elem)
    return MRES_INVALID_ARGS;
  if (_MVS_MFUNC_STACK_CHECK_EMPTY_(stack))
    return MRES_COULDNT_COMPLETE;
  *(mbptr_t *)elem = (mbptr_t)stack->buf + stack->sp * stack->elem_len;
  return MRES_SUCCESS;
}

mResult_t mvs_stack_clear(MVSStack *stack) {
  if (!stack)
    return MRES_INVALID_ARGS;
  stack->sp = (msize_t)(-1);
  return MRES_SUCCESS;
}

mResult_t mvs_stack_destroy(MVSStack *stack) {
  if (!stack)
    return MRES_INVALID_ARGS;
  if (!stack->external_buffer)
    free(stack->buf);
  free(stack);
  return MRES_SUCCESS;
}
