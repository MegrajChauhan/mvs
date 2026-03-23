.intel_syntax noprefix
#include "mvs_config.h"

// mqword_t mvs_obtain_flags_regr()
.global mvs_obtain_flags_regr

// void mvs_compare_two_values(mqword_t val1, mqword_t val2)
.global mvs_compare_two_values

mvs_obtain_flags_regr:
   pushfq          // push the flags register unto the stack
   pop rax         // pop the flags
   ret             // return the flags value in RAX

#if defined(_USE_LINUX_)
mvs_compare_two_values:
   cmp rdi, rsi
   mov rdi, rdx
   ret         // Now use the above to obtain the flags register
#endif
