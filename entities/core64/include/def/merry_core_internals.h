#ifndef _MERRY_INTERNALS_
#define _MERRY_INTERNALS_

#include <mvs_types.h>

typedef union MerryCoreFlagsRegr MerryCoreFlagsRegr;
typedef struct MerryCoreFFlagsRegr MerryCoreFFlagsRegr;
typedef struct MerryCoreStackFrame MerryCoreStackFrame;

struct MerryCoreFFlagsRegr {
  mbyte_t zf: 1;  // zero flag
  mbyte_t sf: 1;  // sign flag
  mbyte_t uof 1; // unordered flag
  mbyte_t of: 1;  // overflow flag
  mbyte_t uf 1;  // underflow flag
  mbyte_t inv: 1; // invalid flag
  mbyte_t res: 2; // reserved
};

union MerryCoreFlagsRegr {
  struct {
#if defined(_MVS_HOST_ARCH_x86_)
    // This defines the flags structure for the AMD64 processors
    unsigned long carry : 1;     /*0th bit is the CF in AMD64 EFlags*/
    unsigned long r1: 1;            /*1 bit reserved here*/
    unsigned long parity : 1;    /*2th bit is the PF*/
    unsigned long r2: 1;            /*1 bit reserved here*/
    unsigned long aux_carry : 1; /*4th bit Aux Carry flag[NOT REALLY NEEDED AS
                                    BCD INSTRUCTIONS ARE NOT SUPPORTED]*/
    unsigned long r3: 1;            /*1 bit reserved here*/
    unsigned long zero : 1;      /*6th bit ZF*/
    unsigned long negative : 1;  /*7th bit SF or NG*/
    unsigned long r4: 2;            /*2 bit reserved here*/
    unsigned long direction
        : 1; /*11th bit is the DF[NOT REALLY USEFUL YET BUT MAYBE WHEN
                IMPLEMENTING STRING RELATED INSTRUCTIONS]*/
    unsigned long overflow : 1; /*10th bit is the OF*/
    unsigned long rem_32: 20;
    unsigned long top_32: 32;
#endif
  } flags;
  mqword_t regr;
};

struct MerryCoreStackFrame {
  maddress_t RET_ADDR;
  maddress_t FRAME_BP;
  maddress_t JMP_TO;
};

#endif
