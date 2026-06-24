#ifndef _MERRY_CORE_
#define _MERRY_CORE_

#include <merry_core_defs.h>
#include <merry_core_instruction_handler.h>
#include <merry_core_internals.h>
#include <merry_core_ram.h>
#include <merry_core_registers.h>
#include <mvs_list.h>
#include <merry_logger.h>
#include <mvs_mapped_memory.h>
#include <mvs_protectors.h>
#include <mvs_stack.h>
#include <merry_arg_parse.h>
#include <merry_config.h>
#include <stdlib.h>

typedef struct MerryCore MerryCore;

struct MerryCore {
  EntityContext ctx;
  MerryConfig conf;
  MerryArgParseResult args;
  MerryCoreRAM *iram, *dram;
  MVSStack *stack_frames;
  MVSMappedMemory *st_page;
  MerryCoreFlagsRegr flags;
  MerryCoreFFlagsRegr fflags;
  mqword_t REGISTER_FILE[MERRY_CORE_REG_COUNT];
  maddress_t BP, SP;
  maddress_t PC;
  MerryHostMemLayout IR;
  mqptr_t stack;
  msize_t ret;
};

msize_t merry_core_create(EntityContext *ctx, mbptr_t *repr, msize_t conf);

msize_t merry_core_prepare(mptr_t repr);

msize_t merry_core_destroy(mptr_t c);

msize_t merry_core_run(mptr_t c);

msize_t merry_core_get_default_setup();

mbool_t merry_core_check_setup(msize_t setup);

msize_t merry_core_get_setup_value(mstr_t key, mstr_t val, mbool_t *correct); 

#endif
