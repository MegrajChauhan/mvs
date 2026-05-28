#include <mvs_graves.h>

typedef msize_t (*mgravesreqhdlr_t)(MVSGravesRequest *);

/*----Request Handlers----*/
_MVS_ATTR_INTERNAL_ msize_t
mvs_request_handle_SPAWN_ENTITY(MVSGravesRequest *req);

/*---Internal State---*/
_MVS_ATTR_INTERNAL_ MVSGraves graves = {0};
_MVS_ATTR_INTERNAL_ mgravesreqhdlr_t request_hdlrs[] = {
    mvs_request_handle_SPAWN_ENTITY};

_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_initialize_logger();
_MVS_ATTR_INTERNAL_ void mvs_graves_shutdown_logger();

/*----Initialization Functions----*/
_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_initialize_logger() {
  mbool_t dbg = graves.cmd_opts->log_lvl == MLOG_DBG ? mtrue : mfalse;
  mthread th;
  if (dbg)
    fprintf(stdout, "<Initialization>: Logger Initialization\n");
  if (!mvs_logger_init(graves.cmd_opts->log_lvl)) {
    if (dbg)
      fprintf(stderr, "<Initialization>: Logger Initialization failed\n");
    return mfalse;
  }
  if (mvs_create_detached_thread(&th, mvs_logger_run, NULL) != MRES_SUCCESS) {
    if (dbg)
      fprintf(stderr, "<Initialization>: Logger Failed to Launch\n");
    mvs_logger_destroy();
    return mfalse;
  }
  mvs_logger_wait_to_launch();
  mvs_log_dbg("Logger initialized and launched");
  return mtrue;
}

_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_initialize_self() {
  mvs_log_dbg("Graves self-initialization begin");
  mvs_log_dbg("Pre-initialization");
  if (mvs_mutex_init(&graves.graves_lock) != MRES_SUCCESS)
    return mfalse;
  if (mvs_cond_init(&graves.graves_cond) != MRES_SUCCESS)
    return mfalse;
  mvs_log_dbg("Checkpoint: Entity List");
  if (mvs_dynamic_listl_create(&graves.entity_list, 10, sizeof(MVSEntity *)) !=
      MRES_SUCCESS)
    return mfalse;
  mvs_log_dbg("Checkpoint: Request Queue Manager");
  if (!(graves.req_queue = mvs_request_queue_manager_create()))
    return mfalse;
  graves.local_API.make_request = mvs_graves_API_make_request;
  graves.entity_count = 0;
  graves.active_entities = 0;
  graves.return_val = 0;
  graves.signature_key = 0;
  atomic_init(&graves.graves_all_entities_launched, mfalse);
  return mtrue;
}

_MVS_ATTR_INTERNAL_ MVSEntity *mvs_graves_create_entity() {
  MVSEntity *entity = (MVSEntity *)malloc(sizeof(MVSEntity));
  if (!entity) {
    mvs_log_err("Failed to create new ENTITY");
    return NULL;
  }
}

/*----Cleanup Functions----*/
_MVS_ATTR_INTERNAL_ void mvs_graves_shutdown_logger() {
  mvs_log_dbg("Shutting Down Logger[Log messages will stop...]");
  mvs_logger_wakeup(mtrue);
  mvs_logger_wait_for_termination();
}

_MVS_ATTR_INTERNAL_ void mvs_graves_shutdown_self() {
  mvs_cond_destroy(&graves.graves_cond);
  mvs_mutex_destroy(&graves.graves_lock);
  if (graves.entity_list)
    mvs_dynamic_listl_destroy(graves.entity_list);
  if (graves.req_queue)
    mvs_request_queue_manager_destroy(graves.req_queue);
}

/*----Utility Functions----*/
_MVS_ATTR_INTERNAL_ mthreadRet_t
mvs_graves_entity_launcher_for_command_execution(mptr_t ent) {
  // This is a special function just for the entities that were launched by the
  // launch commands if -es flag was provided
  mvs_mutex_lock(&graves.graves_lock);
  mvs_cond_wait(&graves.graves_cond, &graves.graves_lock);
  mvs_mutex_unlock(&graves.graves_lock);
  if (graves.cmd_opts->ensure_success && !graves.graves_all_entities_launched)
    return NULL; // failed
  return mvs_graces_entity_launcher(ent);
}

_MVS_ATTR_INTERNAL_ msize_t mvs_graves_execute_launch_commands() {
  mvs_log_dbg("Checkpoint: Executing Launch Commands");
  mvs_log_dbg("Launch commands given: %zu commands",
              graves.cmd_opts->entities_to_spawn);
  MVSEntitySpawnCommand *command = graves.cmd_opts->spawn_commands;
  MVSEntitySpawnCommand *curr = command;
  while (curr) {
  }
}

/*----Entry/Exit Functions----*/
_MVS_ATTR_INTERNAL_ void mvs_die() {
  mvs_log_dbg("Finalizing Shutdown");
  mvs_graves_shutdown_self();
  mvs_graves_shutdown_logger();
}

void mvs_run(MVSArgParseResult *opts) {
  graves.cmd_opts = opts;
  if (!mvs_graves_initialize_logger())
    exit(1);
  // Initialize Graves now
  if (!mvs_graves_initialize_self()) {
    mvs_log_dbg("Graves self-initialization failed");
    goto __mvs_run_exit;
  }
  mvs_log_dbg("Graves self-initialization COMPLETED");
  mvs_log_dbg("Checkpoint: Graves Starting");
  msize_t ret = mvs_graves_run();
  mvs_log_dbg("Checkpoint: Graves Stopped[RET=%zu]", ret);
  mvs_log_dbg("Self-shutdown begin");
__mvs_run_exit:
  mvs_die();
  exit(graves.return_val);
}

_MVS_ATTR_INTERNAL_ msize_t mvs_graves_run() {
  // First launch all the entities as commanded
}
