#include <mvs_graves_entity_utils.h>

MVSEntity *mvs_graves_entity_utils_create_entity() {
  MVSEntity *ent = (MVSEntity *)malloc(sizeof(MVSEntity));
  if (!ent) {
    mvs_log_err("Failed to allocate memory for ENTITY");
    return NULL;
  }
  ent->entity_repr = NULL;
  return ent;
}

void mvs_graves_entity_utils_destroy_entity(MVSEntity *ent) {
  if (ent->config.config & MVS_CONF_ENTITY_LOCAL_ENTITY_LIST_ENABLE)
    mvs_dynamic_listl_destroy(ent->entity_local_list);
  free(ent);
  // freeing entity_repr is not the task for this function
}

mbool_t mvs_graves_entity_utils_config_valid(msize_t conf) {
  msize_t local_ent_list_en = conf & MVS_CONF_ENTITY_LOCAL_ENTITY_LIST_ENABLE;
  msize_t can_spawn_entity = conf & MVS_CONF_ENTITY_CAN_SPAWN_ENTITY;
  if (local_ent_list_en && !can_spawn_entity)
    return mfalse;
  if ((conf & MVS_CONF_ENTITY_ENTITY_TRACKING_LIM) &&
      (!can_spawn_entity || (can_spawn_entity && !local_ent_list_en)))
    return mfalse;
  return mtrue;
}

mbool_t mvs_graves_entity_utils_properties_valid(msize_t props) {
  return mtrue;
}

mbool_t mvs_graves_entity_utils_init_entity(MVSEntity *ent, EntityContext *ctx,
                                            msize_t EID, mEntity_t type,
                                            msize_t conf, msize_t props,
                                            mqword_t in_conf,
                                            APIRequestResponse *resp) {
  if (EID >= _MVS_CONSTANT_ENTITY_COUNT_) {
    *resp = _API_REQ_RESPONSE_BAD_(API_REQ_RESPONSE_INVALID_EID);
    return mfalse;
  }
  EntityRegistryEntry *entry = mvs_registry_get_entry(EID);
  if (!entry) {
    *resp = _API_REQ_RESPONSE_BAD_(API_REQ_RESPONSE_UNREGISTERED_EID);
    return mfalse;
  }
  if (!in_conf && entry->get_default_setup)
    in_conf = entry->get_default_setup();
  else if (in_conf && entry->check_setup &&
           (entry->check_setup(in_conf) != mtrue)) {
    *resp = _API_REQ_RESPONSE_BAD_(API_REQ_RESPONSE_INVALID_SETUP);
    return mfalse;
  }
  if (!mvs_graves_entity_utils_config_valid(conf)) {
    *resp = _API_REQ_RESPONSE_BAD_(API_REQ_RESPONSE_INVALID_CONFIG);
    return mfalse;
  }
  if (!mvs_graves_entity_utils_properties_valid(props)) {
    *resp = _API_REQ_RESPONSE_BAD_(API_REQ_RESPONSE_INVALID_PROPERTY);
    return mfalse;
  }
  mbptr_t repr;
  if (entry->create(ctx, &repr, in_conf) != 0) {
    *resp = _API_REQ_RESPONSE_BAD_(API_REQ_RESPONSE_ENTITY_INIT_FAILED);
    return mfalse;
  }

  // This is basic initialization only
  ent->type = type;
  ent->EID = EID;
  atomic_init(&ent->state.state, MENTITY_INIT);
  ent->config.config = conf;
  ent->properties.props = props;
  ent->entity_repr = repr;

  ent->entity_local_list_history = 0;
  ent->entity_local_list_tracks = 0;
  ent->entity_local_list = NULL;
  *resp = _API_REQ_RESPONSE_GOOD_();
  return mtrue;
}

mbool_t
mvs_graves_entity_utils_prepare_entity(MVSEntity *ent, APIRequestResponse *resp,
                                       msize_t local_ent_list_size_lim) {
  // After initialization, this function prepares the entity
  mqword_t conf = ent->config.config;

  msize_t lim = 20; // default limit
  if (conf & MVS_CONF_ENTITY_ENTITY_TRACKING_LIM) {
    if (!local_ent_list_size_lim) {
      *resp = _API_REQ_RESPONSE_BAD_(API_REQ_RESPONSE_INVALID_DATA);
      return mfalse;
    }
    lim = local_ent_list_size_lim;
  }
  if (conf & MVS_CONF_ENTITY_LOCAL_ENTITY_LIST_ENABLE) {
    mResult_t res;
    if ((res = mvs_dynamic_listl_create(&ent->entity_local_list, lim,
                                        sizeof(MVSEntity *))) != MRES_SUCCESS) {
      *resp = _API_REQ_RESPONSE_BAD_(mvs_convert_result_to_req_response(res));
      return mfalse;
    }
    ent->entity_local_list_size_lim = lim;
  }
  atomic_store_explicit(&ent->state.state, MENTITY_READY_TO_RUN);
  *resp = _API_REQ_RESPONSE_GOOD_();
  return mtrue;
}
