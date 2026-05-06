#ifndef _MERRY_CORE_
#define _MERRY_CORE_

#include <mvs_types.h>
#include <mvs_tools.h>
#include <mvs_list.h>
#include <mvs_mapped_memory.h>
#include <mvs_protectors.h>
#include <mvs_stack.h>
#include <merry_core_defs.h>
#include <merry_core_instruction_handler.h>
#include <merry_core_internals.h>
#include <merry_core_ram.h>
#include <merry_core_registers.h>
#include <merry_core_sysint.h>
#include <merry_flags_regr.h>
#include <merry_logger_subsystem.h>
#include <merry_input_reader.h>
#include <merry_arg_parse.h>
#include <mvs_entity_interface.h>
#include <stdlib.h>

typedef struct MerryCore MerryCore;

struct MerryCore {
  MVSEntityContext ctx;
  MerryParseResult res;
  MerryCoreRAM *iram, *dram;
  MVSStack *stack_frames;
  MerryInput *input;
  MVSMappedMemory *st;
  MerryCoreFlagsRegr flags;
  MerryCoreFFlagsRegr fflags;
  mcond_t cond;
  mqword_t REGISTER_FILE[MERRY_CORE_REG_COUNT];
  maddress_t BP, SP;
  maddress_t PC;
  MVSHostMemLayout IR;
  mqptr_t stack;
  msize_t ret;
  msize_t signature;
};

msize_t merry_core_create(MVSEntityContext *ctx, mbptr_t* repr, mstr_t *argv, msize_t argv, msize_t sig);

msize_t merry_core_destroy(mptr_t c);

msize_t merry_core_run(mptr_t c);

mbool_t merry_core_preinit(MerryCore *core, mstr_t *argv, msize_t argv, msize_t sig);

#endif
