#include <mvs_graves.h>

typedef msize_t (*mgravesreqhdlr_t)(MVSGravesRequest *);

/*----Request Handlers----*/
_MVS_ATTR_INTERNAL_ msize_t
mvs_request_handle_SPAWN_ENTITY(MVSGravesRequest *req);

_MVS_ATTR_INTERNAL_ MVSGraves graves = {0};
_MVS_ATTR_INTERNAL_ mgravesreqhdlr_t request_hdlrs[] = {
    mvs_request_handle_SPAWN_ENTITY};

/*----Other Functions----*/
_MVS_ATTR_INTERNAL_ msize_t mvs_graves_launch_logger(mLogLvl_t lvl);
_MVS_ATTR_INTERNAL_ void mvs_graves_shutdown_logger();
_MVS_ATTR_INTERNAL_ msize_t mvs_graves_init(MVSArgParseResult *opts);
_MVS_ATTR_INTERNAL_ void mvs_graves_destroy();
_MVS_ATTR_INTERNAL_ msize_t mvs_graves_run();
_MVS_ATTR_INTERNAL_ msize_t mvs_graves_launch_all_entities();
_MVS_ATTR_INTERNAL_ MVSEntity *mvs_graves_get_entity(MVSEntityIdentity *iden);
_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_check_config_valid(msize_t ID,
                                                          mqword_t conf);
_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_check_properties_valid(msize_t ID,
                                                              mqword_t prop);
_MVS_ATTR_INTERNAL_ MVSEntity *mvs_graves_find_free_entity();
_MVS_ATTR_INTERNAL_ MVSEntity *mvs_graves_create_new_entity();
_MVS_ATTR_INTERNAL_ void mvs_graves_initialize_new_entity(MVSEntity *entity,
                                                          msize_t ID,
                                                          mqword_t conf,
                                                          mqword_t prop);
_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_track_new_entity(MVSEntity *entity);
_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_launch_new_entity(MVSEntity *entity);
_MVS_ATTR_INTERNAL_ mthreadRet_t mvs_graves_entity_launcher(mptr_t ent);

_MVS_ATTR_INTERNAL_ msize_t mvs_graves_launch_logger(mLogLvl_t lvl) {
  mbool_t dbg = (lvl == MLOG_DBG) ? mtrue : mfalse;
  mthread_t th;
  if (dbg)
    fprintf(stdout, "<Graves>: Launching Logger[LOGLVL:%u]\n", lvl);
  if (mvs_logger_init(lvl) != MRES_SUCCESS) {
    fprintf(stderr, "<Graves>: Failed to launch Logger\n");
    return 1;
  }
  if (mvs_create_detached_thread(&th, mvs_logger_run, NULL) != MRES_SUCCESS) {
    fprintf(stderr, "<Graves>: Failed to launch Logger\n");
    mvs_logger_destroy();
    return 1;
  }
  mvs_logger_wait_to_launch();
  mvs_log_dbg("Logger Launched!");
  return 0;
}

_MVS_ATTR_INTERNAL_ void mvs_graves_shutdown_logger() {
  mvs_logger_wakeup(mtrue);          // Stop the logger
  mvs_logger_wait_for_termination(); // wait for proper termination
  mvs_logger_destroy();
}

_MVS_ATTR_INTERNAL_ msize_t mvs_graves_init(MVSArgParseResult *opts) {
  mResult_t res;
  if ((res = mvs_cond_init(&graves.graves_cond)) != MRES_SUCCESS)
    return 1;
  if ((res = mvs_mutex_init(&graves.graves_lock)) != MRES_SUCCESS) {
    mvs_cond_destroy(&graves.graves_cond);
    return 1;
  }
  if ((res = mvs_dynamic_listl_create(&graves.entity_list,
                                      _MVS_CONSTANT_ENTITY_LIST_INITIAL_LEN_,
                                      sizeof(MVSEntity *))) != MRES_SUCCESS) {
    mvs_cond_destroy(&graves.graves_cond);
    mvs_mutex_destroy(&graves.graves_lock);
    return 1;
  }
  if ((graves.req_queue = mvs_request_queue_manager_create()) == NULL) {
    mvs_cond_destroy(&graves.graves_cond);
    mvs_mutex_destroy(&graves.graves_lock);
    mvs_dynamic_listl_destroy(graves.entity_list);
    return 1;
  }
  graves.entity_count = 0;
  graves.active_entities = 0;
  graves.return_val = 0;
  graves.cmd_opts = opts;
  graves.local_API.make_request = mvs_graves_API_make_request;
  mvs_log_dbg("Entities Registered: %zu", mvs_registry_get_count());
  return 0;
}

_MVS_ATTR_INTERNAL_ void mvs_graves_destroy() {
  // This is called only after everything else has stopped
  mvs_cond_destroy(&graves.graves_cond);
  mvs_mutex_destroy(&graves.graves_lock);
  mvs_dynamic_listl_destroy(graves.entity_list);
  mvs_request_queue_manager_destroy(graves.req_queue);
}

_MVS_ATTR_INTERNAL_ msize_t mvs_graves_run() {
  /*
   * Here, the following steps are performed:
   * 1. Launch all of the entities as requested from the command line arguments
   * 2. (optional/in-future)if no entities were requested, provide a command
   * interpreter
   * 3. Then wait for requests and serve them as they come
   */
  mvs_log_dbg("Graves Active and Running...");
  msize_t launch_res = mvs_graves_launch_all_entities();
  MVSGravesRequest *req;
  if (launch_res == 1)
    return 3; // failure
  else if (launch_res == 2) {
    // Launch Command Interpreter since no entities were provided
    // This could be a future addition
    goto __mvs_graves_terminate; // just die for now
  }
  while (mtrue) {
    mvs_mutex_lock(&graves.graves_lock);
    if (!graves.active_entities) {
      // There are no active entities running
      // Based on what there currently is: either launch a command interpreter
      // or just terminate
      break; // and i choose terminate ]=)
    }
    if ((req = mvs_request_queue_manager_dequeue_request(graves.req_queue)) ==
        NULL) {
      // probably empty
      mvs_cond_wait(&graves.graves_cond, &graves.graves_lock);
    } else {
      MVSEntity *entity = mvs_graves_get_entity(req->iden);
      if (!entity) {
        mvs_log_err("Invalid Request Made: ID=%zu, UID=%zu", req->iden->ID,
                    req->iden->UID);
        req->RESULT[0] = _MVS_CONSTANT_REQUEST_REQ_INVALID_;
      } else {
        // handle the request
        if (req->type >= MREQ_COUNT) {
          mvs_log_err("Invalid Request: ID=%zu, UID=%zu, REQ=%zu",
                      req->iden->ID, req->iden->UID, req->type);
          req->RESULT[0] = _MVS_CONSTANT_REQUEST_REQ_TYPE_INVALID_;
        } else {
          msize_t res = request_hdlrs[req->type](req);
          req->RESULT[0] = _MVS_CONSTANT_REQUEST_REQ_SUCCESS_;
          req->RESULT[1] = res;
          mvs_log_dbg("Request Served:ID=%zu, UID=%zu, REQ=%zu(RET=%zu)",
                      req->iden->ID, req->iden->UID, req->type, res);
        }
      }
      atomic_store_explicit(&req->request_served, mtrue, memory_order_release);
      if (req->wakeup_cond)
        mvs_cond_signal(req->wakeup_cond);
    }
    mvs_mutex_unlock(&graves.graves_lock);
  }
__mvs_graves_terminate:
  mvs_log_dbg("Graves Deactivated...(RETURN CODE=%zu)", graves.return_val);
  return graves.return_val;
}

_MVS_ATTR_INTERNAL_ msize_t mvs_graves_launch_all_entities() {
  return 2; // nothing just yet
}

_MVS_ATTR_INTERNAL_ MVSEntity *mvs_graves_get_entity(MVSEntityIdentity *iden) {
  if (iden->ID >= graves.entity_count || iden->UID >= graves.active_entities)
    return NULL; // invalid identity
  MVSEntity *entity;
  if (mvs_dynamic_listl_ref_of(graves.entity_list, &entity, iden->ID) !=
      MRES_SUCCESS)
    return NULL; // this shall never be executed
  return entity;
}

void mvs_run(MVSArgParseResult *opts) {
  if (mvs_graves_launch_logger(opts->log_lvl) != 0) {
    fprintf(stderr, "<Graves>: Logger failed to initialize.\nTerminating...\n");
    exit(1);
  }
  mvs_log_dbg("Initializing Graves...");
  if (mvs_graves_init(opts) != 0) {
    mvs_log_dbg("Graves failed to initialize.\nTerminating...");
    mvs_graves_shutdown_logger();
    exit(2);
  }
  mvs_log_dbg("Graves Successfully Initialized...");
  msize_t res = mvs_graves_run();
  mvs_log_dbg("Shutting Down Graves");
  mvs_graves_destroy();
  mvs_log_dbg("Shutting Down Logger");
  mvs_graves_shutdown_logger();
  exit(res);
}

msize_t mvs_graves_API_make_request(MVSEntityIdentity *hdlr,
                                    MVSGravesRequest *req) {
  if (!hdlr || !req)
    return 2;
  mvs_mutex_lock(&graves.graves_lock);
  msize_t ret = 0;
  /*
   * I realize that it would be a smart move to verify hdlr and req here instead
   * of the serve loop but this is supposed to be a quick call.
   */
  /*
   * hdlr isn't really important here. In future, it could help keep accounting
   * information.
   */
  if (req->wakeup_cond) {
    if (mvs_request_queue_manager_enqueue_request(graves.req_queue, req) != 0)
      ret = 0;
    else
      ret = 1;
  } else {
    if (mvs_request_queue_manager_enqueue_request_async(graves.req_queue,
                                                        req) != 0)
      ret = 0;
    else
      ret = 1;
  }
  mvs_cond_signal(&graves.graves_cond);
  mvs_mutex_unlock(&graves.graves_lock);
  return ret;
}

/*
 * The reason that there is an ID here is because some configs and properties
 * might be entity specific This allows me to make sure to check it in the
 * future. Or even provide a simple request to check the capability of an entity
 * Thus, it can also be said that there are MVS defined configs of an entity but
 * the entity can be configured as well. And users can also use the command line
 * to disable or enable some configuration
 */
_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_check_config_valid(msize_t ID,
                                                          mqword_t conf) {
  // there is nothing to check right now
  return mtrue;
}

_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_check_properties_valid(msize_t ID,
                                                              mqword_t prop) {
  // there is nothing to check
  return mtrue;
}

_MVS_ATTR_INTERNAL_ MVSEntity *mvs_graves_find_free_entity() {
  /*
   * This is as simple as going through each entity and finding a free one
   * I realize that this method doesn't scale well with respect to the number of
   * entities but it wouldn't be genius to have a separate list to store the IDs
   * of free entities either. The number of entities heavily influences the
   * performance. For high number of entities, keeping a cache like that may
   * prove to be worth it but it increases the chances of fragmentation too
   */
  if (graves.entity_count == graves.active_entities)
    return NULL; // no free entity
  MVSEntity *entity;
  for (msize_t i = 0; i < graves.entity_count; i++) {
    mvs_dynamic_listl_ref_of(graves.entity_list, &entity, i);
    if (!entity->entity_repr)
      return entity;
  }
  return NULL;
}

_MVS_ATTR_INTERNAL_ MVSEntity *mvs_graves_create_new_entity() {
  // This just creates a new entity
  MVSEntity *entity = (MVSEntity *)malloc(sizeof(MVSEntity));
  if (!entity) {
    mvs_log_err("Failed to Create New Entity");
    return NULL;
  }
  return entity;
}

_MVS_ATTR_INTERNAL_ void mvs_graves_initialize_new_entity(MVSEntity *entity,
                                                          msize_t ID,
                                                          mqword_t conf,
                                                          mqword_t props) {
  entity->type = MENTITY_LOCAL;
  entity->EID = ID;
  atomic_store_explicit(&entity->state.state, MENTITY_STOPPED,
                        memory_order_relaxed);
  entity->properties.props = props;
  entity->config.config = conf;
}

_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_track_new_entity(MVSEntity *entity) {
  // This just adds the entity to the list and tracks it
  if (mvs_dynamic_listl_push(graves.entity_list, &entity) != MRES_SUCCESS) {
    mvs_log_err("Failed to track an entity");
    // What to do if this section is ever executed?
    // Free the entity? I don't see any other way
    // But that implies destroying the entity_repr that was created
    // It isn't smart to get the entry from the registry again and again
    // So, instead, the entity will only initialize before launch
    return mfalse;
  }
  entity->identity.ID = graves.entity_count++;
  return mtrue;
}

_MVS_ATTR_INTERNAL_ mbool_t mvs_graves_launch_new_entity(MVSEntity *entity) {
  MVSEntityRegistryEntry *entry = mvs_registry_get_entry(entity->EID);
  MVSEntityContext context;
  context.self = &entity->identity;
  context.API = graves.local_API;
  mbptr_t entity_repr;
  if (entry->create(&context, &entity_repr, NULL, 0) != 0) {
    return mfalse;
  }
  entity->entity_repr = (mptr_t)entity_repr;
  mthread_t th;
  if (mvs_create_detached_thread(&th, mvs_graves_entity_launcher, entity) !=
      MRES_SUCCESS) {
    mvs_log_err("Failed to Launch Entity");
    entry->destroy(entity_repr);
    return mfalse;
  }
  entity->identity.UID = graves.active_entities++;
  return mtrue;
}

_MVS_ATTR_INTERNAL_ mthreadRet_t mvs_graves_entity_launcher(mptr_t ent) {
  MVSEntity *entity = (MVSEntity *)ent;
  MVSEntityRegistryEntry *entry = mvs_registry_get_entry(entity->EID);
  atomic_store_explicit(&entity->state.state, MENTITY_RUNNING,
                        memory_order_release);
  while (mtrue) {
    switch (entry->exec(entity->entity_repr)) {
    case 0:
      goto __mvs_graves_terminate_entity;
    case 2: {
      atomic_store_explicit(&entity->state.state, MENTITY_BUSY,
                            memory_order_release);
      break;
    }
    default:
      atomic_store_explicit(&entity->state.state, MENTITY_RUNNING,
                            memory_order_release);
      break;
    }
  }
__mvs_graves_terminate_entity:
  msize_t ret = entry->destroy(entity->entity_repr);
  entity->entity_repr = NULL;
  mvs_mutex_lock(&graves.graves_lock);
  graves.active_entities--;
  if (!graves.active_entities)
    mvs_cond_signal(&graves.graves_cond);
  mvs_mutex_unlock(&graves.graves_lock);
  mvs_log_dbg("Entity[ID=%zu,UID=%zu] exited with code %zu",
              entity->identity.ID, entity->identity.UID, ret);
  return NULL;
}

_MVS_ATTR_INTERNAL_ msize_t
mvs_request_handle_SPAWN_ENTITY(MVSGravesRequest *req) {
  /*
   * The steps to do this:
   * 1. Verify the arguments of the request
   * 2. Check if there is a free entity
   * 3. If there is a free entity use it and launch the entity
   * 4. else create a new entity, initialize it, and launch it
   * It is to be noted that it won't be that simple.
   * Here, more checks are missing
   */
  msize_t ID = req->args.spawn_entity.ID;
  mvs_log_dbg("Serving Request[SPAWN_ENTITY]: ID=%zu, UID=%zu for EID=%zu",
              req->iden->ID, req->iden->UID, ID);
  MVSEntityRegistryEntry *entry = mvs_registry_get_entry(ID);
  if (!entry) {
    mvs_log_err("Entity[%zu] didn't register to MVS...", ID);
    return _MVS_CONSTANT_REQUEST_REQ_ENTITY_NOT_REGISTERED_;
  }
  if (ID >= _MVS_CONSTANT_ENTITY_COUNT_) {
    mvs_log_err("Invalid EID[%zu] provided for SPAWNING[ID=%zu, UID=%zu]", ID,
                req->iden->ID, req->iden->UID);
    return _MVS_CONSTANT_REQUEST_REQ_INVALID_EID_;
  }
  if (!mvs_graves_check_config_valid(ID, req->args.spawn_entity.config)) {
    mvs_log_err("Invalid Config provided for SPAWNING[ID=%zu, UID=%zu]",
                req->iden->ID, req->iden->UID);
    return _MVS_CONSTANT_REQUEST_REQ_INVALID_CONFIG_;
  }
  if (!mvs_graves_check_properties_valid(ID,
                                         req->args.spawn_entity.properties)) {
    mvs_log_err("Invalid Properties provided for SPAWNING[ID=%zu, UID=%zu]",
                req->iden->ID, req->iden->UID);
    return _MVS_CONSTANT_REQUEST_REQ_INVALID_PROPERTY_;
  }
  MVSEntity *entity = mvs_graves_find_free_entity();
  if (!entity) {
    // gonna have to create one
    entity = mvs_graves_create_new_entity();
    if (!entity) {
      req->RESULT[2] = errno;
      return _MVS_CONSTANT_REQUEST_REQ_SYS_FAILURE_;
    }
    mvs_graves_initialize_new_entity(entity, ID, req->args.spawn_entity.config,
                                     req->args.spawn_entity.properties);
    if (!mvs_graves_track_new_entity(entity)) {
      req->RESULT[2] = errno;
      return _MVS_CONSTANT_REQUEST_REQ_SYS_FAILURE_;
    }
  }
  if (!mvs_graves_launch_new_entity(entity))
    return _MVS_CONSTANT_REQUEST_REQ_FAILED_LAUNCH_;
  mvs_log_dbg(
      "Serving Request[SPAWN_ENTITY] Success: ID=%zu, UID=%zu for EID=%zu",
      req->iden->ID, req->iden->UID, ID);
  mvs_log_dbg("New Entity: ID=%zu, UID=%zu", entity->identity.ID,
              entity->identity.UID);
  return _MVS_CONSTANT_REQUEST_REQ_SUCCESS_;
}
