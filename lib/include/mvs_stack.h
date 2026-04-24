#ifndef _MVS_STACK_
#define _MVS_STACK_

#include <mvs_results.h>
#include <mvs_tools.h>
#include <mvs_types.h>
#include <stdlib.h>
#include <string.h>

#define _MVS_MFUNC_STACK_CHECK_FULL_(stack)                                    \
  (((stack)->sp + 1) >= (stack)->sp_max)
#define _MVS_MFUNC_STACK_CHECK_EMPTY_(stack) ((stack)->sp == (msize_t)(-1))
#define _MVS_MFUNC_STACK_CHECK_CAPACITY_(stack, n) (((stack)->sp + 1) >= (n))
#define _MVS_MFUNC_STACK_CHECK_CAPACITY_LEFT_(stack, n)                        \
  (((stack)->sp - (stack)->sp_max) >= (n))

// The stacks are going to be static only
typedef struct MVSStack MVSStack;
struct MVSStack {
  mptr_t buf;
  msize_t sp;
  msize_t cap;
  msize_t elem_len;
  msize_t sp_max;
  mbool_t external_buffer;
};

mResult_t mvs_stack_init(MVSStack **stack, msize_t cap, msize_t elem_len);
mResult_t mvs_stack_init_using_external_buf(MVSStack **stack, mptr_t buf,
                                            msize_t len, msize_t elem_len);
mResult_t mvs_stack_push(MVSStack *stack, mptr_t value);
mResult_t mvs_stack_pop(MVSStack *stack, mptr_t elem);
mResult_t mvs_stack_top(MVSStack *stack, mptr_t elem);
mResult_t mvs_stack_clear(MVSStack *stack);
mResult_t mvs_stack_destroy(MVSStack *stack);

#endif
