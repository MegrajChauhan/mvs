#include <merry_core.h>
#include <merry_core_instruction_list.h>

mbool_t merry_core_preinit(MerryCore *core,mstr_t *argv, msize_t argv, msize_t sig) {
	merry_set_signature(sig);
	// parse arguments and setup
	if (!merry_parse_arg(&core->res, argv, argc, sig))
	  return mfalse;

	// read the input file
	if ((core->input = merry_input_init()) == NULL)
		return mfalse;

	if (!merry_input_read(core->input, core->res->inp_file_path)) {
		merry_input_destroy(core->input);
		return mfalse;
	}
	
	return mtrue;
}

msize_t merry_core_create(MVSEntityContext *ctx, mbptr_t* repr, mstr_t *argv, msize_t argv, msize_t sig) {
  MerryCore *c = (MerryCore *)malloc(sizeof(MerryCore));
  if (!c) {
    return 1;
  }
  if (!merry_core_preinit(c, argc, argc, sig)) {
    MERR("Core Creation Failed");
  	free(c);
    return 1;
  }
  if ((mvs_mapped_memory_create(&c->st, 0)) != MRES_SUCCESS) {
    MERR("Failed to initialize the stack");
	merry_input_destroy(c->input);
    free(c);
    return 1;
  }
  if (mvs_mapped_memory_map(c->st, _MERRY_CORE_STACK_LEN_) != MRES_SUCCESS) {
    MERR("Failed to initialize the stack");  	
    mvs_mapped_memory_destroy(c->st);
	merry_input_destroy(c->input);
    free(c);
    return 1;
  }
  mvs_mapped_memory_obtain_ptr(c->st, (mbptr_t *)&c->stack);
  if ((mvs_stack_init(
           &c->stack_frames, _MERRY_CORE_CALL_DEPTH_, sizeof(MerryCoreStackFrame))) != MRES_SUCCESS) {
    MERR("Failed to initialize the stack frame list");
    mvs_mapped_memory_unmap(c->st);
    mvs_mapped_memory_destroy(c->st);
	merry_input_destroy(c->input);
    free(c);
    return 1;
  }

  if ((c->iram = merry_core_memory_init()) == NULL) {
    MERR("Failed to initialize the Instruction RAM");
    mvs_mapped_memory_unmap(c->st);
    mvs_mapped_memory_destroy(c->st);
	merry_input_destroy(c->input);
    free(c);
    return 1; 
  }

  if (!(merry_core_memory_populate(c->iram, c->input->instruction_len, c->input->instructions))) {
    MERR("Failed to initialize the Instruction RAM");
    mvs_mapped_memory_unmap(c->st);
    mvs_mapped_memory_destroy(c->st);
    merry_core_memory_destroy(c->iram);
	merry_input_destroy(c->input);
    free(c);
    return 1; 
  }

  if ((c->dram = merry_core_memory_init()) == NULL) {
    MERR("Failed to initialize the Data RAM");
    mvs_mapped_memory_unmap(c->st);
    mvs_mapped_memory_destroy(c->st);
    merry_core_memory_destroy(c->iram);
	merry_input_destroy(c->input);
    free(c);
    return 1; 
  }

  if (!(merry_core_memory_populate(c->iram, c->input->instruction_len, c->input->instructions))) {
    MERR("Failed to initialize the Instruction RAM");
    mvs_mapped_memory_unmap(c->st);
    mvs_mapped_memory_destroy(c->st);
    merry_core_memory_destroy(c->iram);
    merry_core_memory_destroy(c->dram);
	merry_input_destroy(c->input);
    free(c);
    return 1; 
  }

  if (mvs_cond_init(&c->cond) != MRES_SUCCESS) {
    MERR("Failed to initialize core synchronization prim");
    mvs_mapped_memory_unmap(c->st);
    mvs_mapped_memory_destroy(c->st);
    merry_core_memory_destroy(c->iram);
    merry_core_memory_destroy(c->dram);
	merry_input_destroy(c->input);
    free(c);
    return 1;
  }
  
  c->SP = 0;
  c->BP = 0;
  c->PC = 0;
  c->signature = sig;
  c->ctx = *ctx;
  *repr = (mbptr_t)c;
  return 0;
}

msize_t merry_core_destroy(mptr_t c) {
  if (!c)
    return 1;
  MerryCore *core = (MerryCore*)c;
  msize_t ret = core->ret;
  mvs_stack_destroy(core->stack_frames);
  merry_core_memory_destroy(core->iram);
  merry_core_memory_destroy(core->dram);
  merry_input_destroy(core->input);
  mvs_mapped_memory_unmap(core->st);
  mvs_mapped_memory_destroy(core->st);
  mvs_cond_destroy(&core->cond);
  free(core);
  return ret;
}

msize_t merry_core_run(mptr_t c) {
  MerryCore *core = (MerryCore*)c;
  mbool_t running = mtrue;
  core->ret = 0;
  while (running) {
    if (!surelyF(merry_core_memory_read_qword(
                    c->iram, c->PC, &c->IR.whole_word))) {
      c->ret = 1;
      break;
    } else {
      running = HDLRS[c->IR.bytes.b0](c);
      c->PC += 8;
    }
  }
  return 0; // terminate
}
