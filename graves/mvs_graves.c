#include <mvs_graves.h>

typedef msize_t (*mgravesreqhdlr_t)(MVSGravesRequest *);

/*----Request Handlers----*/
_MVS_ATTR_INTERNAL_ msize_t
mvs_request_handle_SPAWN_ENTITY(MVSGravesRequest *req);

/*---Internal State---*/
_MVS_ATTR_INTERNAL_ MVSGraves graves = {0};
_MVS_ATTR_INTERNAL_ mgravesreqhdlr_t request_hdlrs[] = {
    mvs_request_handle_SPAWN_ENTITY,
};

#define _MVS_CONSTANT_PARSE_ARG_OPTION_COUNT_ 8

// TODO: move this somewhere else
_MVS_ATTR_INTERNAL_ MVSArgOption opts[_MVS_CONSTANT_PARSE_ARG_OPTION_COUNT_] = {
   {"help message", "-h", 2, mtrue, mvs_HELP_MSG},
   {"help message", "--help", 6, mtrue, mvs_HELP_MSG},
   {"version information", "-v", 2, mtrue, mvs_VERSION},
   {"version information", "--version", 9, mtrue, mvs_VERSION}, 
   {"set log level", "-log", 4, mfalse, mvs_LOG_LVL},	
   {"Provide entity spawn commands", "-spawn", 6, mfalse, mvs_SPAWN_ENTITY_COMMAND},
   {"Provide spawn list", "-slist", 6, mtrue, mvs_SLIST},
   {"Ensure all spawn commands are executed", "-es", 3, mtrue, mvs_ES}
};

/*
 * MVS starts as such:
 * Firstly, the command line options are parsed. Once all of the command line
 * options have been parsed, Graves is launched. Here is how Graves initializes
 * everything:
 * 1. The logger is first initialized and launched
 * 2. Than SLIST file is parsed if provided.
 * 3. Then the internals of Graves is initialized.
 * 4. If SLIST was provided, those entities are first launched.
 * 5. The entities provided in the command line are launched afterwards.
 * 6. Once all entities have been launched, Graves will start fulfilling
 * requests.
 * */

_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_launch_logger();
_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_make_sense_of_cmd_opts();
_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_self_initialize();
_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_pre_init(mstr_t *argv, msize_t argc);
_MVS_ATTR_INTERNAL_ void mvs_graves_init_system_config();
_MVS_ATTR_INTERNAL_ MVSEntity *
mvs_graves_make_command_entity(MVSEntitySpawnCommand *cmd);
_MVS_ATTR_INTERNAL_ MVSEntity *mvs_graves_make_slist_entity(MVSSlistCommand *c);
_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_launch_entities();
_MVS_ATTR_INTERNAL_ void mvs_graves_stop_logger();
_MVS_ATTR_INTERNAL_ void mvs_graves_clean_cmd_initializations();
_MVS_ATTR_INTERNAL_ void mvs_graves_self_destroy();
_MVS_ATTR_INTERNAL_ void mvs_graves_cleanup_entities();
_MVS_ATTR_INTERNAL_ mthreadRet_t mvs_graves_entity_launcher_wait(mptr_t e);
_MVS_ATTR_INTERNAL_ mthreadRet_t mvs_graves_entity_launcher_nowait(mptr_t e);
_MVS_ATTR_INTERNAL_ void mvs_graves_run();

/*----Initialization Functions----*/
_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_launch_logger() {
  if (!mvs_logger_init(graves.cmd_opts.log_lvl)) {
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
  // Here, Graves will initialize various things based on the command line
  // options
  mvs_log_dbg("Making sense of command line options");
  if (graves.cmd_opts.ensure_success) {
    mvs_log_dbg("ensure_success=true");
    if (mvs_barrier_init(&graves.wait_barrier) != MRES_SUCCESS) {
      mvs_log_err("Failed to setup wait-barrier");
      return mfalse;
    }
  }
  if (graves.cmd_opts.slist) {
    mvs_log_dbg("slist=%s", graves.cmd_opts.slist);
    if (!mvs_slist_make_commands(&graves.slist, graves.cmd_opts.slist,
                                 &graves.cmd_opts, &graves.config)) {
      if (graves.cmd_opts.ensure_success) {
        mvs_log_err("Failed to make commands from SLIST");
        return mfalse;
      } else {
        mvs_log_warn("Failed to make commands from SLIST");
        graves.cmd_opts.slist = NULL;
      }
    }
  }
  return mtrue;
}

_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_self_initialize() {
  mvs_log_dbg("Graves self initialization");
  mcond_t cond;
  mmutex_t lock;
  MVSBarrier sync_barrier;
  MVSGravesEntityList *elist = NULL;
  msize_t total_ecount = graves.cmd_opts.entities_to_spawn;
  if (graves.cmd_opts.slist)
    total_ecount += mvs_slist_get_command_count(&graves.slist);
  mvs_log_dbg("Total ecount=%zu", total_ecount);
  if (mvs_cond_init(&cond) != MRES_SUCCESS) {
    mvs_log_err("Graves self-initialization failure");
    return mfalse;
  }
  if (mvs_mutex_init(&lock) != MRES_SUCCESS) {
    mvs_log_err("Graves self-initialization failure");
    mvs_cond_destroy(&cond);
    return mfalse;
  }
  if (mvs_barrier_init(&sync_barrier) != MRES_SUCCESS) {
    mvs_log_err("Graves self-initialization failure");
    mvs_cond_destroy(&cond);
    mvs_mutex_destroy(&lock);
    return mfalse;
  }
  if (total_ecount && (elist = mvs_graves_entity_list_create(total_ecount, (mqword_t)(-1))) ==
      NULL) {
    mvs_log_err("Graves entity list initialization failed");
    mvs_cond_destroy(&cond);
    mvs_mutex_destroy(&lock);
    mvs_barrier_destroy(&sync_barrier);
    return mfalse;
  }
  if ((graves.req_queue = mvs_request_queue_manager_create()) == NULL) {
    mvs_log_err("Graves request queue manager initialization failed");
    mvs_cond_destroy(&cond);
    mvs_mutex_destroy(&lock);
    mvs_barrier_destroy(&sync_barrier);
    mvs_graves_entity_list_destroy(elist);
    return mfalse;
  }
  graves.graves_cond = cond;
  graves.graves_lock = lock;
  graves.entity_list = elist;
  graves.sync_barrier = sync_barrier;
  graves.entity_created = 0;

  graves.return_val = 0;
  return mtrue;
}

_MVS_ATTR_INTERNAL_ void mvs_graves_init_system_config() {
   // This may involve reading configuration files
   graves.config.MAX_EID = 0;
}

_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_pre_init(mstr_t *argv, msize_t argc) {
   mvs_graves_init_system_config();
   graves.local_API = (GravesAPI){ 
		  .make_request = mvs_graves_make_request,
		  .register_component = mvs_register_component,
		  .LOG = mvs_log_dbg,
		  .VLOG = mvs_vlog,
		  .check_request_status = mvs_request_check_status,
		  .get_request_response = mvs_request_get_response,
		  .get_request_result = mvs_request_get_result,
		  .make_request_SPAWN_ENTITY = mvs_create_req_SPAWN_ENTITY,
   };
   if (!mvs_rlist_init(&graves.rlist))
		   return mfalse;
   if (!mvs_rlist_read(&graves.rlist)) {
	       mvs_rlist_destroy(&graves.rlist);
		   return mfalse;
   }
   graves.config.MAX_EID = mvs_rlist_get_count(&graves.rlist);
   if (!mvs_registry_init(&graves.config)) {
     mvs_rlist_destroy(&graves.rlist);
	 return mfalse;
   }
   if (!mvs_rlist_register_entities(&graves.rlist, &graves.local_API)) {
		mvs_rlist_destroy(&graves.rlist);
		mvs_registry_destroy();
		return mfalse;
   }
   MVSArgParse parser = _MVS_MFUNC_ARG_PARSE_INIT_(argc, argv, _MVS_CONSTANT_PARSE_ARG_OPTION_COUNT_);
   mvs_graves_arg_parse_set_default(&graves.cmd_opts);
   if (!mvs_parse_all_arg(&parser, opts, &graves.cmd_opts, mvs_HELP_MSG)) {
   	fprintf(stderr, "Terminating...\n");
    mvs_rlist_destroy(&graves.rlist);
   	return mfalse;
   }
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
  e->identity = (MVSEntityIdentity){.ID = graves.entity_created,
                                    .UID = graves.entity_created};
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
  if (cmd->EID >= graves.config.MAX_EID) {
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

  e->identity = (MVSEntityIdentity){.ID = graves.entity_created,
                                    .UID = graves.entity_created};
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

_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_launch_entities() {
  // There are two types of entities to launch
  // One is provided from command line
  // Other is from slist if provided
  mvs_log_dbg("launching entities");
  mbool_t must_succeed = graves.cmd_opts.ensure_success;
  MVSDynamicListLinear *commands = NULL;
  msize_t slist_len;
  if (graves.cmd_opts.slist) {
    // SLIST was provided
	mvs_log_dbg("SLIST entities:");
    commands = mvs_slist_get_commands(&graves.slist);
    slist_len = mvs_slist_get_command_count(&graves.slist);
    for (msize_t i = 0; i < slist_len; i++) {
      MVSSlistCommand *c =
          *(MVSSlistCommand **)mvs_dynamic_listl_ref_of_unsafe(commands, i);
      MVSEntity *e = mvs_graves_make_slist_entity(c);
      c->skip = mtrue;
      if (!e) {
		mvs_log_dbg("SLIST entity[%zu]= failed to initialize", i);
        if (must_succeed)
          return mfalse;
        continue;
      }
      // This will never fail because the list was successfully preallocated
      mvs_graves_entity_list_add_entity(graves.entity_list, e);
      c->skip = mfalse;
      e->from_slist = mtrue;
      mvs_log_dbg("SLIST entity[%zu]= successfully initialized", i);
      graves.entity_created++;
    }
  }
  mvs_log_dbg("SPAWN COMMAND entities:");
  MVSEntitySpawnCommand *cmd = graves.cmd_opts.spawn_commands;
  while (cmd) {
    MVSEntitySpawnCommand *nxt = cmd->nxt_command;
    for (msize_t i = 0; i < cmd->instances; i++) {
      MVSEntity *e = mvs_graves_make_command_entity(cmd);
      if (!e) {
		mvs_log_dbg("SP_COM entity[%zu]= failed to initialize", i);
        if (must_succeed)
          return mfalse;
        continue;
      }
      mvs_graves_entity_list_add_entity(graves.entity_list, e);
      e->from_slist = mfalse;
      mvs_log_dbg("SP_COM entity[%zu]= successfully initialized", i);
      graves.entity_created++;
    }
	free(cmd);
	cmd = nxt;
  }
  // Now launch all of the entities
  mvs_log_dbg("Initialized %zu entities", graves.entity_created);
  mvs_log_dbg("Launching...");
  mthexec_t func = (must_succeed) ? mvs_graves_entity_launcher_wait
                                  : mvs_graves_entity_launcher_nowait;
  graves.all_launch_success = mfalse;
  for (msize_t i = 0; i < graves.entity_created; i++) {
    mvs_log_dbg("launching ENTITY[%zu]",i);
    mthread_t th;
    MVSEntity *ent = mvs_graves_entity_list_get_entity(graves.entity_list, i);
    MVSSlistCommand *c = NULL;
    msize_t j = 0;
    if (commands) {
      for (; j < slist_len; j++) {
        c = *(MVSSlistCommand **)mvs_dynamic_listl_ref_of_unsafe(commands, i);
        if (!c->skip)
          break;
      }
    }
    if (mvs_create_detached_thread(&th, func, ent) != MRES_SUCCESS) {
	  mvs_log_dbg("launching ENTITY[%zu]: failed", i);
      if (must_succeed) {
        for (msize_t j = 0; j < i; j++) {
          mvs_barrier_signal(&graves.wait_barrier);
        }
        for (msize_t j = 0; j < i; j++) {
          mvs_barrier_wait(&graves.wait_barrier);
        }
        return mfalse;
      }
      if (c) {
        c->launch_success = mfalse;
        c->skip = mtrue; // to skip the entity next iter
      }
      mvs_registry_get_entry(ent->EID)->destroy(ent->entity_repr);
      ent->entity_repr = NULL;
      continue;
    }
    if (c) {
	  mvs_log_dbg("launching ENTITY[%zu]: success", i);
      mvs_log_dbg("SLIST ENTITY: original ID=%zu, after launch=%zu", j, i);
	  c->launch_success = mtrue;
      c->skip = mtrue; // done with this one
      c->ID_after_launch = i;
    }
    mvs_graves_entity_list_register_active_entity(graves.entity_list);
  }
  // Now there is the task of populating the local list.
  // After the local list is populated, the entities can be released
  // Just go through the commands one by one
  mvs_log_dbg("Populating Local List");
  if (commands) {
    for (msize_t i = 0; i < slist_len; i++) {
      mvs_log_dbg("For ENTITY[%zu]:", i);
      MVSSlistCommand *cmd =
          *(MVSSlistCommand **)mvs_dynamic_listl_ref_of_unsafe(commands, i);
      if (cmd->launch_success) {
		mvs_log_dbg("ENTITY[%zu]: initialized and launched", i);
        if (cmd->local_list) {
		  mvs_log_dbg("ENTITY[%zu]: local list enabled", i);
          MVSDynamicListLinear *ll = cmd->local_list;
          MVSEntity *ent = mvs_graves_entity_list_get_entity(
              graves.entity_list, cmd->ID_after_launch);
          for (msize_t j = 0; j < mvs_dynamic_listl_size_unsafe(ll); j++) {
            MVSSlistCommand *cmd_to_add = *(
                MVSSlistCommand **)mvs_dynamic_listl_ref_of_unsafe(commands, j);
            if (cmd_to_add->launch_success) {
		      mvs_log_dbg("ENTITY[%zu]: ID=%zu is running: adding to local ID=%zu", i, j, j);
              MVSEntity *ent = mvs_graves_entity_list_get_entity(
                  graves.entity_list, cmd_to_add->ID_after_launch);
              MVSLocalListEntry new_entry = (MVSLocalListEntry){
                  .ID = ent->identity.ID, .UID = ent->identity.UID};
              mvs_dynamic_listl_push(ent->entity_local_list, &new_entry);
            } else {
              // The launch was a failure so what can be done?
              // Let's give it ID=-1 and UID=-1
		      mvs_log_dbg("ENTITY[%zu]: ID=%zu is not running: local ID=%zu is empty", i, j, j);
              MVSLocalListEntry new_entry = (MVSLocalListEntry){
                  .ID = (mqword_t)(-1), .UID = (mqword_t)(-1)};
              mvs_dynamic_listl_push(ent->entity_local_list, &new_entry);
            }
          }
        }
      }
    }
  }
  graves.all_launch_success = mtrue;
  // Now gotta release all of the workers
  if (must_succeed) {
    mvs_log_dbg("Releasing all entities...");
    for (msize_t i = 0; i < graves.entity_created; i++) {
      mvs_barrier_signal(&graves.wait_barrier);
    }
  }
  mvs_log_dbg("Active entities: %zu", mvs_graves_entity_list_get_active_entity_count(graves.entity_list));
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
  if (graves.cmd_opts.ensure_success)
    mvs_barrier_destroy(&graves.wait_barrier);
  if (graves.cmd_opts.slist)
    mvs_slist_destroy(&graves.slist);
}

_MVS_ATTR_INTERNAL_ void mvs_graves_self_destroy() {
  mvs_cond_destroy(&graves.graves_cond);
  mvs_mutex_destroy(&graves.graves_lock);
  mvs_barrier_destroy(&graves.sync_barrier);
  mvs_graves_entity_list_destroy(graves.entity_list);
  mvs_request_queue_manager_destroy(graves.req_queue);
}

_MVS_ATTR_INTERNAL_ void mvs_graves_cleanup_entities() {
  if (!graves.entity_list)
    return;
  MVSDynamicListLinear *list =
      mvs_graves_entity_list_get_entity_list(graves.entity_list);
  for (msize_t i = 0; i < mvs_dynamic_listl_size_unsafe(list); i++) {
    MVSEntity *ent = *(MVSEntity **)mvs_dynamic_listl_ref_of_unsafe(list, i);
    mvs_graves_entity_utils_destroy_entity(ent);
  }
}

/*----Worker functions----*/
_MVS_ATTR_INTERNAL_ mthreadRet_t mvs_graves_entity_launcher_wait(mptr_t e) {
  // This will just wait and then go back to running
  // mvs_graves_entity_launcher_nowait
  mvs_barrier_wait(&graves.wait_barrier);
  if (!graves.all_launch_success) {
    MVSEntity *ent = (MVSEntity *)e;
    EntityRegistryEntry *entry = mvs_registry_get_entry(ent->EID);
    entry->destroy(ent->entity_repr);
    mvs_barrier_signal(&graves.wait_barrier);
    return NULL;
  }
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
  mvs_graves_entity_list_unregister_active_entity(graves.entity_list);
  atomic_store_explicit(&ent->state, MENTITY_STOPPED, memory_order_release);
  if (mvs_graves_entity_list_get_active_entity_count(graves.entity_list) == 0)
    mvs_barrier_signal(&graves.sync_barrier);
  return NULL;
}

/*----Executing Functions----*/
void mvs_done() {
  mvs_graves_cleanup_entities();
  mvs_graves_self_destroy();
  mvs_graves_clean_cmd_initializations();
  mvs_graves_stop_logger();
  mvs_rlist_destroy(&graves.rlist);
  mvs_registry_destroy();
}

void mvs_run(mstr_t *argv, msize_t argc) {
  if (!mvs_graves_pre_init(argv, argc))
    exit(-1);

  if (!mvs_graves_launch_logger()) {
    mvs_rlist_destroy(&graves.rlist);
	mvs_registry_destroy();
    exit(-1);
  }
  mvs_log_dbg("Entities registered: %zu", mvs_registry_get_count());
  mvs_log_dbg("checkpoint: pre-initialization");
  if (!mvs_graves_make_sense_of_cmd_opts()) {
    mvs_log_dbg("failed pre-initialization");
    mvs_rlist_destroy(&graves.rlist);
	mvs_registry_destroy();
    exit(-1);
  }
  mvs_log_dbg("checkpoint: initialization");
  if (!mvs_graves_self_initialize()) {
    mvs_log_dbg("failed initialization");
    mvs_rlist_destroy(&graves.rlist);
	mvs_registry_destroy();
    exit(-1);
  }
  mvs_log_dbg("checkpoint: entity launch");
  if (graves.cmd_opts.entities_to_spawn > 0 || graves.cmd_opts.slist) {
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
  exit(graves.return_val);
}

_MVS_ATTR_INTERNAL_ void mvs_graves_run() {
  mvs_log_dbg("checkpoint: Graves is running");
  MVSGravesRequest *req = NULL;
  while (mvs_graves_entity_list_non_empty(graves.entity_list)) {
    mvs_barrier_wait(&graves.sync_barrier);
    if (!mvs_graves_entity_list_non_empty(graves.entity_list))
		break;
    req = mvs_request_queue_manager_dequeue_request(graves.req_queue);
    mvs_log_dbg("Graves: Obtained new request");
	mvs_mutex_lock(&graves.graves_lock);
    MVSEntity *entity;
    if (req->type >= MREQ_COUNT) {
      /*
       * invalid requests are not fatal at all
       * */
      req->response = _API_REQ_RESPONSE_BAD_(API_REQ_RESPONSE_INVALID_REQ);
      goto __mvs_graves_run_exit;
    }
    if (!mvs_graves_entity_list_validate_ID(graves.entity_list,
                                            req->iden->ID)) {
      req->response = _API_REQ_RESPONSE_BAD_(
          API_REQ_RESPONSE_INVALID_CRED); // somehow the identity was
                                          // mutated(this shouldn't happen)
      goto __mvs_graves_run_exit;
    }
    entity =
        mvs_graves_entity_list_get_entity(graves.entity_list, req->iden->ID);
    if (entity->identity.UID != req->iden->UID) {
      req->response = _API_REQ_RESPONSE_BAD_(API_REQ_RESPONSE_INVALID_CRED);
      goto __mvs_graves_run_exit;
    }
    request_hdlrs[req->type](req);
  __mvs_graves_run_exit:
	mvs_mutex_unlock(&graves.graves_lock);
    atomic_store_explicit(&req->request_served, mtrue, memory_order_release);
    if (req->wakeup_cond)
      mvs_cond_signal(req->wakeup_cond);
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
  if (mvs_request_queue_manager_enqueue_request(graves.req_queue, *req) != 0)
    return API_RES_RETRY;
  if ((*req)->wakeup_cond) {
    mvs_mutex_lock(&graves.graves_lock);
	mvs_barrier_signal(&graves.sync_barrier);
	mvs_cond_wait((*req)->wakeup_cond, &graves.graves_lock);
	mvs_mutex_unlock(&graves.graves_lock);
    return API_RES_SUCCESS;
  } else {
    mvs_barrier_signal(&graves.sync_barrier);
  }
  return API_RES_SUCCESS;
}

/*----Request Handlers----*/
_MVS_ATTR_INTERNAL_ msize_t
mvs_request_handle_SPAWN_ENTITY(MVSGravesRequest *req) {
  req->response = _API_REQ_RESPONSE_GOOD_();
  return 0;
}
