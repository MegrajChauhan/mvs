#include <mvs_graves.h>
#include <mvs_graves_request_handlers_list.h>

/*---Internal State---*/
_MVS_ATTR_INTERNAL_ MVSGraves graves = {0};

/*
 * The initialization of MVS goes something like this:
 * First of all, the list file is read and all the entities are loaded into
 * registry list. Once the registry list is populated, Graves registers each
 * entity one by one. Finally, the command line options are parsed. If a spawn
 * list is given, it is parsed. Then and only then, Graves starts
 * self-initialization. Here, in future, configurations might be added. The
 * logger is launched, Graves finishes initialization and all entities are
 * launched . Then Graves starts its own loop.
 * */

_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_launch_logger();
_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_make_sense_of_cmd_opts();
_MVS_ATTR_INTERNAL_ void mvs_graves_set_accounting();
_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_init_sync();
_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_init_components();
_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_self_initialize();
_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_populate_registry();
_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_pre_init(mstr_t *argv, msize_t argc);
_MVS_ATTR_INTERNAL_ MVSEntity *
mvs_graves_make_command_entity(MVSEntitySpawnCommand *cmd);
_MVS_ATTR_INTERNAL_ MVSEntity *mvs_graves_make_slist_entity(MVSSlistCommand *c);
_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_initialize_slist_entries();
_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_initialize_cmd_line_launch_entries();
_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_launch_entities();
_MVS_ATTR_INTERNAL_ void mvs_graves_stop_logger();
_MVS_ATTR_INTERNAL_ void mvs_graves_clean_cmd_initializations();
_MVS_ATTR_INTERNAL_ void mvs_graves_destroy_sync();
_MVS_ATTR_INTERNAL_ void mvs_graves_destroy_components();
_MVS_ATTR_INTERNAL_ void mvs_graves_self_destroy();
_MVS_ATTR_INTERNAL_ mthreadRet_t mvs_graves_entity_launcher_wait(mptr_t e);
_MVS_ATTR_INTERNAL_ mthreadRet_t mvs_graves_entity_launcher_nowait(mptr_t e);
_MVS_ATTR_INTERNAL_ void mvs_graves_run();

/*----Initialization Functions----*/
_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_launch_logger() {
  if (!mvs_logger_init(graves.state.cmd_opts.log_lvl, &graves.state.config)) {
    fprintf(stderr, "<INIT>: Failed to initialize logger\n");
    return mfalse;
  }
  mthread_t th;
  if (mvs_create_detached_thread(&th, mvs_logger_run, NULL) != MRES_SUCCESS) {
    fprintf(stderr, "<INIT>: Failed to launch logger\n");
    mvs_logger_destroy();
    return mfalse;
  }
  mvs_logger_wait_to_launch();
  mvs_log_dbg("Logger up and running!");
  return mtrue;
}

_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_make_sense_of_cmd_opts() {
  mvs_log_dbg("Making sense of command line options");
  if (graves.state.cmd_opts.slist) {
    mvs_log_dbg("slist=%s", graves.state.cmd_opts.slist);
    if (!mvs_slist_make_commands(
            &graves.components.slist, graves.state.cmd_opts.slist,
            &graves.state.cmd_opts, &graves.state.config)) {
      mvs_log_err("Failed to make commands from SLIST");
      return mfalse;
    }
  }
  return mtrue;
}

_MVS_ATTR_INTERNAL_ void mvs_graves_set_accounting() {
  graves.accounting.requests_served = 0;
  graves.accounting.requests_success = 0;
}

_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_init_sync() {
  if (mvs_barrier_init(&graves.sync.sleep_barrier) != MRES_SUCCESS) {
    mvs_log_err("[SYNC]: Failed to setup protocols");
    return mfalse;
  }
  if (mvs_barrier_init(&graves.sync.release_barrier) != MRES_SUCCESS) {
    mvs_log_err("[SYNC]: Failed to setup protocols");
    mvs_barrier_destroy(&graves.sync.sleep_barrier);
    return mfalse;
  }
  return mtrue;
}

_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_init_components() {
  msize_t total_ecount = graves.state.cmd_opts.entities_to_spawn;
  if (graves.state.cmd_opts.slist)
    total_ecount += mvs_slist_get_command_count(&graves.components.slist);
  mvs_log_dbg("Total ecount=%zu", total_ecount);
  if (total_ecount &&
      (graves.components.entity_list = mvs_graves_entity_list_create(
           total_ecount, (mqword_t)(-1))) == NULL) {
    mvs_log_err("Graves entity list initialization failed");
    return mfalse;
  }
  if ((graves.components.queue_manager =
           mvs_request_queue_manager_create(&graves.state.config)) == NULL) {
    mvs_log_err("Graves request queue manager initialization failed");
    mvs_graves_entity_list_destroy(graves.components.entity_list);
    return mfalse;
  }
  return mtrue;
}

_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_self_initialize() {
  mvs_log_dbg("Graves self initialization");
  mvs_graves_set_accounting();
  if (!mvs_graves_init_sync())
    return mfalse;
  if (!mvs_graves_init_components()) {
    mvs_graves_destroy_sync();
    return mfalse;
  }
  return mtrue;
}

_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_populate_registry() {
  // After RLIST is populated from a list file, Graves will populate the
  // registry.
  for (msize_t i = 0; i < graves.state.config.MAX_EID; i++) {
    mentityRegister_t registrar;
    mbool_t sys = mfalse;
    if (!(registrar =
              mvs_rlist_get_registrar(&graves.components.rlist, i, &sys))) {
      fprintf(stderr, "[RLIST]: Couldn't obtain the registrar for EID=%zu\n",
              i);
      return mfalse;
    }
    msize_t ret;
    if ((ret = registrar(i, graves.local_API)) != 0) {
      fprintf(stderr, "[RLIST]: Registration of EID=%zu failed with return %zu",
              i, ret);
      return mfalse;
    }
  }
  return mtrue;
}

_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_pre_init(mstr_t *argv, msize_t argc) {
  mvs_system_config_populate_default(&graves.state.config);
  graves.local_API = (GravesAPI){
      .make_request = mvs_graves_make_request,
      .register_component = mvs_register_component,
      .LOG_DBG = mvs_log_dbg,
	  .LOG_NOTE = mvs_log_note,
	  .LOG_ERR = mvs_log_err,
	  .LOG_WARN = mvs_log_warn,
      .VLOG = mvs_vlog,
      .check_request_status = mvs_request_check_status,
      .get_request_response = mvs_request_get_response,
      .get_request_result = mvs_request_get_result,
      .make_request_SPAWN_ENTITY = mvs_create_req_SPAWN_ENTITY,
  };
  if (!mvs_rlist_init(&graves.components.rlist))
    return mfalse;
  if (!mvs_rlist_load_from_list_file(&graves.components.rlist,
                                     "./entities.l")) {
    mvs_rlist_destroy(&graves.components.rlist);
    return mfalse;
  }
  graves.state.config.MAX_EID = mvs_rlist_get_count(&graves.components.rlist);
  if (!mvs_registry_init(&graves.state.config)) {
    mvs_rlist_destroy(&graves.components.rlist);
    return mfalse;
  }
  if (!mvs_graves_populate_registry()) {
    mvs_rlist_destroy(&graves.components.rlist);
    return mfalse;
  }
  MVSArgParse parser = _MVS_MFUNC_ARG_PARSE_INIT_(
      argc, argv, _MVS_CONSTANT_PARSE_ARG_OPTION_COUNT_);
  if (!mvs_parse_all_arg(&parser, &graves.state.cmd_opts)) {
    fprintf(stderr, "Terminating...\n");
    mvs_rlist_destroy(&graves.components.rlist);
    return mfalse;
  }
  mvs_arg_parse_populate_config(&graves.state.cmd_opts, &graves.state.config);
  return mtrue;
}

_MVS_ATTR_INTERNAL_ MVSEntity *
mvs_graves_make_slist_entity(MVSSlistCommand *c) {
  MVSEntity *e = mvs_graves_entity_utils_create_entity();
  EntityRegistryEntry *entry = mvs_registry_get_entry(c->EID);
  if (!e)
    return NULL;
  if (!c->config_provided) {
    c->config = 0; // TODO: provide something default here
  } else if (!mvs_graves_entity_utils_config_valid(c->config)) {
    mvs_log_err("<INIT>: config provided for ID=%zu is invalid[SLIST]",
                c->config);
    mvs_graves_entity_utils_destroy_entity(e);
    return NULL;
  }
  if (!c->properties_provided) {
    c->properties = 0; // TODO: provide something default here
  } else if (!mvs_graves_entity_utils_properties_valid(c->properties)) {
    mvs_log_err("<INIT>: properties provided for ID=%zu is invalid[SLIST]",
                c->config);
    mvs_graves_entity_utils_destroy_entity(e);
    return NULL;
  }
  if (!c->setup_provided) {
    if (!entry->get_default_setup)
      c->setup = 0;
    else
      c->setup = entry->get_default_setup();
  } else {
    if (entry->check_setup) {
      if (!entry->check_setup(c->setup)) {
        mvs_log_err("<INIT>: setup provided for ID=%zu is invalid[SLIST]",
                    c->config);
        mvs_graves_entity_utils_destroy_entity(e);
        return NULL;
      }
    }
  }
  EntityContext ctx;
  ctx.self = &e->identity;
  ctx.argv = &c->args;
  ctx.argc = 1;
  ctx.slist = mtrue;
  if (!mvs_graves_entity_utils_init_entity_hotpath(
          e, &ctx, c->EID, MENTITY_LOCAL, c->config, c->properties, c->setup)) {
    mvs_log_err("<INIT>: Failed to initialize entity");
    mvs_graves_entity_utils_destroy_entity(e);
    return NULL;
  }
  if (!mvs_graves_entity_utils_prepare_entity_hotpath(
          e, c->limits.local_list_lim)) {
    mvs_log_err("<INIT>: Failed to initialize entity");
    entry->destroy(e->entity_repr);
    mvs_graves_entity_utils_destroy_entity(e);
    return NULL;
  }
  return e;
}

_MVS_ATTR_INTERNAL_ MVSEntity *
mvs_graves_make_command_entity(MVSEntitySpawnCommand *cmd) {
  if (cmd->EID >= graves.state.config.MAX_EID) {
    mvs_log_err("<INIT>: EID=%zu provided for launch command is not valid",
                cmd->EID);
    return NULL;
  }
  EntityRegistryEntry *entry = mvs_registry_get_entry(cmd->EID);
  if (!entry) {
    mvs_log_err("<INIT>: EID=%zu provided for launch command is not registered",
                cmd->EID);
    return NULL;
  }
  msize_t config = 0;     // TODO: get something default
  msize_t properties = 0; // TODO: something default
  msize_t setup = (entry->get_default_setup) ? entry->get_default_setup() : 0;

  MVSEntity *e = mvs_graves_entity_utils_create_entity();
  if (!e)
    return NULL;
  EntityContext ctx;
  ctx.self = &e->identity;
  ctx.argv = cmd->argv;
  ctx.argc = cmd->argc;
  ctx.slist = mfalse;
  if (!mvs_graves_entity_utils_init_entity_hotpath(
          e, &ctx, cmd->EID, MENTITY_LOCAL, config, properties, setup)) {
    mvs_log_err("<INIT>: Failed to initialize entity");
    mvs_graves_entity_utils_destroy_entity(e);
    return NULL;
  }
  if (!mvs_graves_entity_utils_prepare_entity_hotpath(e, (mqword_t)(-1))) {
    mvs_log_err("<INIT>: Failed to initialize entity");
    entry->destroy(e->entity_repr);
    mvs_graves_entity_utils_destroy_entity(e);
    return NULL;
  }
  return e;
}

_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_initialize_slist_entries() {
  MVSDynamicListLinear *commands =
      mvs_slist_get_commands(&graves.components.slist);
  msize_t slist_len = mvs_slist_get_command_count(&graves.components.slist);
  for (msize_t i = 0; i < slist_len; i++) {
    MVSSlistCommand *c =
        *(MVSSlistCommand **)mvs_dynamic_listl_ref_of_unsafe(commands, i);
    MVSEntity *e = mvs_graves_make_slist_entity(c);
    if (!e) {
      mvs_log_dbg("SLIST entity[%zu]= failed to initialize", i);
      return mfalse;
    }
    // This will never fail because the list was successfully preallocated
    mvs_graves_entity_list_add_entity(graves.components.entity_list, e);
    mvs_log_dbg("SLIST entity[%zu]= successfully initialized", i);
  }
  return mtrue;
}

_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_initialize_cmd_line_launch_entries() {
  MVSEntitySpawnCommand *curr = graves.state.cmd_opts.spawn_commands;
  while (curr) {
    MVSEntitySpawnCommand *nxt = curr->nxt_command;
    for (msize_t i = 0; i < curr->instances; i++) {
      MVSEntity *e = mvs_graves_make_command_entity(curr);
      if (!e) {
        mvs_log_dbg("entity failed to initialize");
        return mfalse;
      }
      // This will never fail because the list was successfully preallocated
      mvs_graves_entity_list_add_entity(graves.components.entity_list, e);
      mvs_log_dbg("entity successfully initialized");
    }
    free(curr);
    curr = nxt;
  }
  return mtrue;
}

_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_populate_local_list() {
  MVSDynamicListLinear *commands =
      mvs_slist_get_commands(&graves.components.slist);
  msize_t slist_len = mvs_slist_get_command_count(&graves.components.slist);
  if (commands) {
    for (msize_t i = 0; i < slist_len; i++) {
      mvs_log_dbg("For ENTITY[%zu]:", i);
      MVSSlistCommand *cmd =
          *(MVSSlistCommand **)mvs_dynamic_listl_ref_of_unsafe(commands, i);
      mvs_log_dbg("ENTITY[%zu]: initialized and launched", i);
      if (cmd->local_list) {
        mvs_log_dbg("ENTITY[%zu]: local list enabled", i);
        MVSDynamicListLinear *ll = cmd->local_list;
        MVSEntity *ent = mvs_graves_entity_list_get_entity_by_ID(
            graves.components.entity_list, i);
        for (msize_t j = 0; j < mvs_dynamic_listl_size_unsafe(ll); j++) {
          msize_t ID_to_add =
              *(msize_t *)mvs_dynamic_listl_ref_of_unsafe(ll, j);
          MVSSlistCommand *cmd_to_add =
              *(MVSSlistCommand **)mvs_dynamic_listl_ref_of_unsafe(commands,
                                                                   ID_to_add);
          mvs_log_dbg("ENTITY[%zu]: ID=%zu is running: adding to local ID=%zu",
                      i, ID_to_add, j);
          MVSEntity *ent_to_add = mvs_graves_entity_list_get_entity_by_ID(
              graves.components.entity_list, ID_to_add);
          if (!mvs_graves_entity_utils_add_to_local_list(ent, ent_to_add)) {
            mvs_log_err(
                "[UNREACHABLE]: mvs_graves_entity_utils_add_to_local_list "
                "failed mvs_graves_populate_local_list");
            // This should be unreachable code because the local list is
            // initialized for exactly the needed number of entries
            return mfalse;
          }
        }
      }
    }
  }
  return mtrue;
}

_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_launch_entities() {
  // There are two types of entities to launch
  // One is provided from command line
  // Other is from slist if provided
  mvs_log_dbg("launching entities");
  graves.state.all_initial_entities_initialized = mfalse;
  graves.state.all_initial_entities_launched = mfalse;
  if (graves.state.cmd_opts.slist) {
    // SLIST was provided
    mvs_log_dbg("SLIST entities:");
    if (!mvs_graves_initialize_slist_entries())
      return mfalse;
  }
  mvs_log_dbg("SPAWN COMMAND entities:");
  if (!mvs_graves_initialize_cmd_line_launch_entries())
    return mfalse;
  graves.state.all_initial_entities_initialized = mtrue;
  mvs_log_dbg("Launching...");
  for (msize_t i = 0; i < mvs_graves_entity_list_get_current_entity_count(
                              graves.components.entity_list);
       i++) {
    mvs_log_dbg("launching ENTITY[%zu]", i);
    mthread_t th;
    MVSEntity *ent = mvs_graves_entity_list_get_entity_by_ID(
        graves.components.entity_list, i);
    if (mvs_create_detached_thread(&th, mvs_graves_entity_launcher_wait, ent) !=
        MRES_SUCCESS) {
      mvs_log_dbg("launching ENTITY[%zu]: failed", i);
      for (msize_t j = 0; j < i; j++)
        mvs_barrier_signal(&graves.sync.sleep_barrier);
      return mfalse;
    }
    mvs_log_dbg("launching ENTITY[%zu]: success", i);
  }
  // Now there is the task of populating the local list.
  // After the local list is populated, the entities can be released
  // Just go through the commands one by one
  mvs_log_dbg("Populating Local List");
  msize_t count = mvs_graves_entity_list_get_current_entity_count(
      graves.components.entity_list);
  if (!mvs_graves_populate_local_list()) {
    for (msize_t j = 0; j < count; j++)
      mvs_barrier_signal(&graves.sync.sleep_barrier);
    return mfalse;
  }
  graves.state.all_initial_entities_launched = mtrue;
  mvs_log_dbg("Releasing all entities...");
  for (msize_t i = 0; i < count; i++) {
    mvs_barrier_signal(&graves.sync.sleep_barrier);
  }
  // TODO: Graves is launching way fast and not letting the entities launch
  for (msize_t i = 0; i < count; i++) {
    mvs_barrier_wait(&graves.sync.release_barrier);
  }
  mvs_log_dbg("Active entities: %zu",
              mvs_graves_entity_list_get_active_entity_count(
                  graves.components.entity_list));
  return mtrue;
}

/*----Cleanup Functions----*/
_MVS_ATTR_INTERNAL_ void mvs_graves_stop_logger() {
  mvs_log_dbg("Shutting down logger[Messages will stop...]");
  mvs_logger_wakeup(mtrue);
  mvs_logger_wait_for_termination();
  return;
}

_MVS_ATTR_INTERNAL_ void mvs_graves_clean_cmd_initializations() {
  if (graves.state.cmd_opts.slist)
    mvs_slist_destroy(&graves.components.slist);
}

_MVS_ATTR_INTERNAL_ void mvs_graves_destroy_sync() {
  mvs_barrier_destroy(&graves.sync.sleep_barrier);
  mvs_barrier_destroy(&graves.sync.release_barrier);
}

_MVS_ATTR_INTERNAL_ void mvs_graves_destroy_components() {
  if (graves.components.entity_list)
    mvs_graves_entity_list_destroy(graves.components.entity_list);
  if (graves.components.queue_manager)
    mvs_request_queue_manager_destroy(graves.components.queue_manager);
}

_MVS_ATTR_INTERNAL_ void mvs_graves_self_destroy() {
  mvs_graves_destroy_sync();
  mvs_graves_destroy_components();
}

/*----Worker functions----*/
_MVS_ATTR_INTERNAL_ mthreadRet_t mvs_graves_entity_launcher_wait(mptr_t e) {
  // This will just wait and then go back to running
  // mvs_graves_entity_launcher_nowait
  mvs_barrier_wait(&graves.sync.sleep_barrier);
  if (!graves.state.all_initial_entities_launched) {
    MVSEntity *ent = (MVSEntity *)e;
    EntityRegistryEntry *entry = mvs_registry_get_entry(ent->EID);
    entry->destroy(ent->entity_repr);
    return NULL;
  }
  mvs_graves_entity_list_register_active_entity(graves.components.entity_list,
                                                (MVSEntity *)e);
  mvs_barrier_signal(&graves.sync.release_barrier);
  return mvs_graves_entity_launcher_nowait(e);
}

_MVS_ATTR_INTERNAL_ mthreadRet_t mvs_graves_entity_launcher_nowait(mptr_t e) {
  MVSEntity *ent = (MVSEntity *)e;
  mvs_log_dbg("ENTITY[%zu:%zu]: running", ent->identity.ID, ent->identity.UID);
  EntityRegistryEntry *entry = mvs_registry_get_entry(ent->EID);
  atomic_store_explicit(&ent->state, MENTITY_RUNNING, memory_order_release);
  while (mtrue) {
    switch (entry->exec(ent->entity_repr)) {
    case 0:
      goto __mvs_graves_entity_launcher_nowait_terminate;
    case 2:
      atomic_store_explicit(&ent->state, MENTITY_BUSY, memory_order_release);
      break;
    default:
      atomic_store_explicit(&ent->state, MENTITY_RUNNING, memory_order_release);
      break;
    }
  }
__mvs_graves_entity_launcher_nowait_terminate:
  mvs_log_dbg("ENTITY[%zu:%zu]: Entity terminated", ent->identity.ID,
              ent->identity.UID);
  entry->destroy(ent->entity_repr);
  ent->entity_repr = NULL;
  mvs_graves_entity_list_unregister_active_entity(graves.components.entity_list,
                                                  ent);
  atomic_store_explicit(&ent->state, MENTITY_STOPPED, memory_order_release);
  if (mvs_graves_entity_list_get_active_entity_count(
          graves.components.entity_list) == 0)
    mvs_barrier_signal(&graves.sync.sleep_barrier);
  return NULL;
}

/*----Executing Functions----*/
void mvs_done() {
  mvs_graves_self_destroy();
  mvs_graves_clean_cmd_initializations();
  mvs_graves_stop_logger();
  mvs_rlist_destroy(&graves.components.rlist);
  mvs_registry_destroy();
}

void mvs_run(mstr_t *argv, msize_t argc) {
  /*
   * Read the list file and populate the registry list. Then load the entities
   * and populate the entity registry. After that, parse the command line
   * options.
   * */
  if (!mvs_graves_pre_init(argv, argc))
    exit(-1);

  /*
   * Safe to launch the logger now
   * */
  if (!mvs_graves_launch_logger()) {
    mvs_rlist_destroy(&graves.components.rlist);
    mvs_registry_destroy();
    exit(-1);
  }

  mvs_log_dbg("Entities registered: %zu", mvs_registry_get_count());
  mvs_log_dbg("checkpoint: pre-initialization");

  /*
   * Based on the command line options provided, initialize Graves
   * This will read SLIST if provided
   * */
  if (!mvs_graves_make_sense_of_cmd_opts()) {
    mvs_log_dbg("failed pre-initialization");
    mvs_rlist_destroy(&graves.components.rlist);
    mvs_registry_destroy();
    exit(-1);
  }
  mvs_log_dbg("checkpoint: initialization");
  /*
   * Finally finish the rest of the initialization
   * */
  if (!mvs_graves_self_initialize()) {
    mvs_log_dbg("failed initialization");
    mvs_rlist_destroy(&graves.components.rlist);
    mvs_registry_destroy();
    exit(-1);
  }
  mvs_log_dbg("checkpoint: entity launch");
  /*
   * It's finally time to launch the entities.
   * For now, if there are 0 spawn commands, then, Graves does nothing but exit.
   * But that may change in the future.
   * */
  if (graves.state.cmd_opts.entities_to_spawn > 0 ||
      graves.state.cmd_opts.slist) {
    if (!mvs_graves_launch_entities()) {
      mvs_log_dbg("failed entity launch");
      goto __mvs_run_exit;
    }
  } else {
    mvs_log_dbg("No entities to launch");
  }
  mvs_log_dbg("checkpoint: graves run");
  mvs_graves_run();
  mvs_log_dbg("checkpoint: graves terminating");
__mvs_run_exit:
  mvs_done();
  exit(0);
}

_MVS_ATTR_INTERNAL_ void mvs_graves_run() {
  mvs_log_dbg("checkpoint: Graves is running");
  MVSGravesRequest *req = NULL;
  while (mvs_graves_entity_list_get_active_entity_count(
      graves.components.entity_list)) {
    mvs_barrier_wait(&graves.sync.sleep_barrier);
    if (!mvs_graves_entity_list_get_active_entity_count(
            graves.components.entity_list))
      break;
    req = mvs_request_queue_manager_dequeue_request(
        graves.components.queue_manager);
	if (!req)
      continue;
    mvs_log_dbg("Graves: Obtained new request");
    MVSEntity *entity;
    if (req->type >= MREQ_COUNT) {
      /*
       * invalid requests are not fatal at all
       * */
      req->response = _API_REQ_RESPONSE_BAD_(API_REQ_RESPONSE_INVALID_REQ);
      goto __mvs_graves_run_exit;
    }
    if (!mvs_graves_entity_list_validate_identity(graves.components.entity_list,
                                                  req->iden)) {
      req->response = _API_REQ_RESPONSE_BAD_(
          API_REQ_RESPONSE_INVALID_CRED); // somehow the identity was
                                          // mutated(this shouldn't happen)
      goto __mvs_graves_run_exit;
    }
    entity = mvs_graves_entity_list_get_entity(graves.components.entity_list,
                                               req->iden);
    request_hdlrs[req->type](&graves, req);
    graves.accounting.requests_success++;
  __mvs_graves_run_exit:
    atomic_store_explicit(&req->request_served, mtrue, memory_order_release);
    graves.accounting.requests_served++;
  }
}

/*----API Functions----*/
_MVS_ATTR_EXPORT_
apiRes_t mvs_graves_make_request(MVSEntityIdentity *iden, GravesRequest **req) {
  /*
   * Should the API perform the validity checks or not?
   * In this function, 'iden' is basically useless.
   * Graves will validate during request fulfillment anyway so should the checks
   * be done here or later?
   * For now, i choose later.
   * */
  if (!req || !iden || !(*req))
    return API_RES_INVALID_ARGS;
  if (mvs_request_queue_manager_enqueue_request(graves.components.queue_manager,
                                                *req) != 0)
    return API_RES_RETRY;
  mvs_barrier_signal(&graves.sync.sleep_barrier);
  return API_RES_SUCCESS;
}
