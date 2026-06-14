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
  atomic_init(&ent->state, MENTITY_INIT);
  ent->config.config = conf;
  ent->properties.props = props;
  ent->entity_repr = repr;

  ent->entity_local_list_history = 0;
  ent->entity_local_list_tracks = 0;
  ent->entity_local_list = NULL;
  *resp = _API_REQ_RESPONSE_GOOD_();
  return mtrue;
}

mbool_t mvs_graves_entity_utils_init_entity_hotpath(MVSEntity *ent,
                                                    EntityContext *ctx,
                                                    msize_t EID, mEntity_t type,
                                                    msize_t conf, msize_t props,
                                                    mqword_t in_conf) {
  EntityRegistryEntry *entry = mvs_registry_get_entry(EID);
  mbptr_t repr;
  if (entry->create(ctx, &repr, in_conf) != 0) {
    return mfalse;
  }

  ent->type = type;
  ent->EID = EID;
  atomic_init(&ent->state, MENTITY_INIT);
  ent->config.config = conf;
  ent->properties.props = props;
  ent->entity_repr = repr;

  ent->entity_local_list_history = 0;
  ent->entity_local_list_tracks = 0;
  ent->entity_local_list = NULL;
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
                                        sizeof(MVSLocalListEntry))) !=
        MRES_SUCCESS) {
      *resp = _API_REQ_RESPONSE_BAD_(mvs_convert_result_to_req_response(res));
      return mfalse;
    }
    ent->entity_local_list_size_lim = lim;
  }
  atomic_store_explicit(&ent->state, MENTITY_READY_TO_RUN,
                        memory_order_release);
  *resp = _API_REQ_RESPONSE_GOOD_();
  return mtrue;
}

mbool_t mvs_graves_entity_utils_prepare_entity_hotpath(
    MVSEntity *ent, msize_t local_ent_list_size_lim) {
  mqword_t conf = ent->config.config;

  msize_t lim = 20;
  if (conf & MVS_CONF_ENTITY_ENTITY_TRACKING_LIM) {
    if (!local_ent_list_size_lim) {
      return mfalse;
    }
    lim = local_ent_list_size_lim;
  }
  if (conf & MVS_CONF_ENTITY_LOCAL_ENTITY_LIST_ENABLE) {
    mResult_t res;
    if ((res = mvs_dynamic_listl_create(&ent->entity_local_list, lim,
                                        sizeof(MVSEntity *))) != MRES_SUCCESS) {
      return mfalse;
    }
    ent->entity_local_list_size_lim = lim;
  }
  atomic_store_explicit(&ent->state, MENTITY_READY_TO_RUN,
                        memory_order_release);
  return mtrue;
}

mbool_t mvs_graves_entity_utils_check_local_list_enable(MVSEntity *ent) {
   return (ent->config.config & MVS_CONF_ENTITY_LOCAL_ENTITY_LIST_ENABLE)? mtrue:mfalse;
}

mbool_t mvs_graves_entity_utils_check_local_list_limit_set(MVSEntity *ent) {
  return (ent->config.config & MVS_CONF_ENTITY_ENTITY_TRACKING_LIM)? mtrue:mfalse;
}

mbool_t mvs_graves_entity_utils_check_local_list_full(MVSEntity *ent) {
  return (ent->entity_local_list_tracks >= ent->entity_local_list_size_lim)? mtrue:mfalse;
}

mbool_t mvs_graves_entity_utils_add_to_local_list(MVSEntity *ent, MVSEntity *to_add) {
   /*
	* It is to be noted that once anything is added to the local list, it cannot be
	* removed unless the entity stops. 
	* */
   if (!mvs_graves_entity_utils_check_local_list_enable(ent))
		return mfalse;
   if (mvs_graves_entity_utils_check_local_list_limit_set(ent) && mvs_graves_entity_utils_check_local_list_full(ent))
		return mfalse;
   if (mvs_dynamic_listl_push(ent->entity_local_list, (mptr_t)&to_add->identity) != MRES_SUCCESS) {
		return mfalse;
   }
   ent->entity_local_list_tracks++;
   ent->entity_local_list_history++;
   return mtrue;
}

mbool_t mvs_graves_entity_utils_clear_local_list(MVSEntity *ent) {
    if (!mvs_graves_entity_utils_check_local_list_enable(ent))
		return mfalse;  
	mvs_dynamic_listl_clear(ent->entity_local_list);
    ent->entity_local_list_history = 0;
	ent->entity_local_list_tracks = 0;
	ent->entity_local_list_size_lim = 0;
	return mtrue;
}

mbool_t mvs_graves_entity_utils_probe_local_list_position(MVSEntity *ent, MVSEntityIdentity *res, msize_t pos) {
   // NOTE: Actually, Graves will perform the following checks properly by itself.
   // Thus, it is absolutely fine to remove them if needed. For now, they just exist
   if (!mvs_graves_entity_utils_check_local_list_enable(ent))
		return mfalse;
   if (ent->entity_local_list_tracks < pos)
		return mfalse;
   *res = *(MVSEntityIdentity*)mvs_dynamic_listl_ref_of_unsafe(ent->entity_local_list, pos);
   return mtrue;
}


