#include <merry_core.h>
#include <merry_core_instruction_list.h>

msize_t merry_core_create(EntityContext *ctx, mbptr_t *repr, msize_t conf) {
  MerryCore *c = (MerryCore *)malloc(sizeof(MerryCore));
  if (!c) {
    ctx->API.LOG_CUSTOM(ctx->self, "Failed to allocate memory for the core");
    return 1;
  }
  if (mvs_mapped_memory_create(&c->st_page) != MRES_SUCCESS) {
    ctx->API.LOG_CUSTOM(ctx->self, "Failed to initialize the stack");
    free(c);
    return 1;
  }
  if (mvs_mapped_memory_map(c->st_page, _MERRY_CORE_STACK_LEN_) !=
      MRES_SUCCESS) {
    ctx->API.LOG_CUSTOM(ctx->self, "Failed to initialize the stack");
    mvs_mapped_memory_destroy(c->st_page);
    free(c);
    return 1;
  }
  mvs_mapped_memory_obtain_ptr(c->st_page, (mbptr_t *)&c->stack, 0);
  if (mvs_stack_init(&c->stack_frames, _MERRY_CORE_CALL_DEPTH_,
                     sizeof(MerryCoreStackFrame)) != MRES_SUCCESS) {
    ctx->API.LOG_CUSTOM(ctx->self, "Failed to initialize the stack trace");
    mvs_mapped_memory_destroy(c->st_page);
    free(c);
    return 1;
  }

  merry_config_init_default(&c->conf);

  c->SP = 0;
  c->BP = 0;
  c->PC = 0;
  c->ctx = *ctx;
  c->iram = NULL;
  c->dram = NULL;
  c->input = NULL;
  *repr = (mbptr_t)c;
  return 0;
}

msize_t merry_core_prepare(mptr_t repr) {
  MerryCore *c = (MerryCore *)repr;
  merry_logger_init(&c->ctx);
  if (c->ctx.slist) {
    if (c->ctx.argc > 0) {
      msize_t count = 0;
      mstr_t *args = merry_get_commands_from_string(*(c->ctx.argv), &count);
      if (!args) {
        MERRY_ERR("Failed to obtain arguments");
        return 1;
      }
      c->ctx.argc = count;
      c->ctx.argv = args;
    }
  }

  MerryArgParser parser = _MERRY_ARG_PARSER_INIT_(c->ctx.argc, c->ctx.argv,
                                                  _MERRY_CMD_OPTION_COUNT_);
  if (!merry_parse_all_arg(&parser, &c->args)) {
    MERRY_ERR("Terminating(Arg parse failed)...\n");
    return 1;
  }
  if (!c->args.input_file) {
    MERRY_ERR("No input file provided for execution!");
    MERRY_NOTE("Terminating....");
    return 1;
  }
  merry_arg_parse_populate_config_from_result(&c->args, &c->conf);

  // all that remains is the parsing of the input file and populating the memory
  c->input = merry_input_init();
  if (!c->input) {
    MERRY_ERR("Couldn't initialize input reader");
    return 1;
  }
  if (!merry_input_read(c->input, c->args.input_file)) {
    MERRY_ERR("Failed to read input file %s", c->args.input_file);
    return 1;
  }
  mbptr_t inst = merry_input_get_instructions(c->input);
  mbptr_t data = merry_input_get_data(c->input);
  msize_t inst_len = merry_input_get_instruction_len(c->input);
  msize_t data_len = merry_input_get_data_len(c->input);
  msize_t entry = merry_input_get_entry(c->input);

  if ((c->iram = merry_core_memory_init()) == NULL) {
    MERRY_ERR("Failed to initialize Instruction Memory");
    return 1;
  }
  if ((c->dram = merry_core_memory_init()) == NULL) {
    MERRY_ERR("Failed to initialize Data Memory");
    return 1;
  }
  if (!merry_core_memory_populate(c->iram, inst_len, inst)) {
    MERRY_ERR("Failed to populate instruction memory");
    return 1;
  }
  if (!merry_core_memory_populate(c->dram, data_len, data)) {
    MERRY_ERR("Failed to populate data memory");
    return 1;
  }
  return 0;
}

msize_t merry_core_destroy(mptr_t core) {
  MerryCore *c = (MerryCore *)core;
  if (!core)
    return 1;
  if (c->ctx.slist) {
    for (msize_t i = 0; i < c->ctx.argc; i++) {
      free(c->ctx.argv[i]);
    }
    free(c->ctx.argv);
  }
  mvs_mapped_memory_destroy(c->st_page);
  mvs_stack_destroy(c->stack_frames);
  merry_core_memory_destroy(c->dram);
  merry_core_memory_destroy(c->iram);
  merry_input_destroy(c->input);
  free(c);
  return 0;
}

msize_t merry_core_run(mptr_t core) {
  MerryCore *c = (MerryCore *)core;
  mbool_t running = mtrue;
  c->ret = 1;
  while (running) {
    if (surelyF(merry_core_memory_read_qword(c->iram, c->PC,
                                             &c->IR.whole_word) != mtrue)) {
      MERRY_ERR("Page Fault: PC=%zu", c->PC);
      c->ret = 0; // stop running
      break;
    } else {
      if (!HDLRS[c->IR.bytes.b0](c))
        c->ret = 0;
      c->PC += 8;
    }
  }
  return c->ret;
}

msize_t merry_core_get_default_setup() { return 0; }

mbool_t merry_core_check_setup(msize_t setup) { return mtrue; }

msize_t merry_core_get_setup_value(mstr_t key, mstr_t val, mbool_t *correct) {
  *correct = mtrue;
  return 0;
}

_MVS_ATTR_EXPORT_
EntityRegistryEntry entity_register() {
  EntityRegistryEntry entry = _API_MAKE_ENTITY_REGISTRY_ENTRY_(
      merry_core_create, merry_core_destroy, merry_core_run,
      merry_core_get_default_setup, merry_core_check_setup,
      merry_core_get_setup_value, merry_core_prepare);
  return entry;
}
