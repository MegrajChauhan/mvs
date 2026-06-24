#include <merry_core.h>
#include <merry_core_instruction_list.h>

msize_t merry_core_create(EntityContext *ctx, mbptr_t *repr, msize_t conf) {
  MerryCore *c = (MerryCore *)malloc(sizeof(MerryCore));
  if (!c) {
    MERRY_ERR("Failed to allocate memory for the core");
	return 1;
  }
  if (mvs_mapped_memory_create(&c->st_page, 0) != MRES_SUCCESS) {
    MERRY_ERR("Failed to initialize the stack");
    free(c);
	return 1;
  }
  if (merry_mapped_memory_map(c->st_page, _MERRY_CORE_STACK_LEN_) !=
      MRES_SUCCESS) {
    MERRY_ERR("Failed to initialize the stack");
    mvs_mapped_memory_destroy(c->st_page);
    free(c);
	return 1;
  }
  mvs_mapped_memory_obtain_ptr(c->st, (mbptr_t *)&c->stack, 0);
  if (mvs_stack_init(
           &c->stack_frames, _MERRY_CORE_CALL_DEPTH_, sizeof(MerryCoreStackFrame)) != MRES_SUCCESS) {
    MERRY_ERR("Failed to initialize the stack trace");
    mvs_mapped_memory_destroy(c->st);
    free(c);
	return 1;
  }

  merry_config_init_default(&c->conf);

  c->SP = 0;
  c->BP = 0;
  c->PC = 0;
  c->ctx = *ctx;
  c->req_list = NULL;
  c->iram = NULL;
  c->dram = NULL;
  *repr = (mbptr_t)c;
  return 0;
}

msize_t merry_core_prepare(mptr_t repr) {
   MerryCore *c = (MerryCore*)repr;

   if (c->ctx.slist) {
     // TODO: Continue from here 
   }

   MerryArgParser parser = _MERRY_ARG_PARSER_INIT_(
       c->ctx.argc, c->ctx.argv, _MERRY_CMD_OPTION_COUNT_);
   if (!merry_parse_all_arg(&parser, &c->args)) {
      MERRY_ERR("Terminating...\n");
      mvs_rlist_destroy(&graves.components.rlist);
      return mfalse;
    }
    mvs_arg_parse_populate_config(&graves.state.cmd_opts, &graves.state.config);
}

void merry_core_destroy(MerryCore *c) {
  if (!c)
    return;

  merry_Interface_list_destroy(c->interfaces);
  merry_CoreProcFrame_stack_destroy(c->stack_frames);
  merry_core_memory_destroy(c->iram);
  merry_core_memory_destroy(c->dram);
  merry_mapped_memory_unmap(c->st);
  merry_mapped_memory_destroy(c->st);
  free(c);
}

msize_t merry_core_run(MerryCore *c) {
  mbool_t running = mtrue;
  c->ret = 0;
  while (running) {
    if (surelyF(merry_core_memory_read_qword(
                    c->iram, c->PC, &c->IR.whole_word) != MRES_SUCCESS)) {
      MERRY_ERR("Page Fault: PC=%zu", c->PC);
      c->ret = 1;
      break;
    } else {
      running = HDLRS[c->IR.bytes.b0](c);
      c->PC += 8;
    }
  }
  return c->ret;
}

mresult_t merry_core_prepare_inst(MerryCore *c, mbptr_t inst, msize_t len) {
  if (!c || !inst)
    return MRES_INVALID_ARGS;
  c->iram = merry_core_memory_init();
  if (!c->iram)
    return MRES_SYS_FAILURE;
  return merry_core_memory_populate(c->iram, len, inst);
}

mresult_t merry_core_prepare_data(MerryCore *c, mbptr_t data, msize_t len) {
  if (!c || !data)
    return MRES_INVALID_ARGS;
  c->dram = merry_core_memory_init();
  if (!c->dram)
    return MRES_SYS_FAILURE;
  return merry_core_memory_populate(c->dram, len, data);
}
