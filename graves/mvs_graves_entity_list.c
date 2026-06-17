#include <mvs_graves_entity_list.h>

MVSGravesEntityList *mvs_graves_entity_list_create(msize_t init,
                                                   msize_t limit) {
  MVSGravesEntityList *list =
      (MVSGravesEntityList *)malloc(sizeof(MVSGravesEntityList));
  if (!list) {
    mvs_log_err("Failed to initialize internal entity list");
    return NULL;
  }

  mbool_t limit_provided = (limit == (mqword_t)(-1)) ? mfalse : mtrue;
  msize_t lim = (limit_provided) ? limit : init;
  if (mvs_dynamic_listl_create(&list->entity_list, lim, sizeof(MVSEntity *)) !=
      MRES_SUCCESS) {
    mvs_log_err("Failed to initialize internal entity list");
    free(list);
    return NULL;
  }

  atomic_init(&list->total_entities_ever_created, 0);
  atomic_init(&list->active_entity_count, 0);
  list->total_entity_count = lim;
  list->current_entity_count = 0;
  list->active_entity_limit = (limit_provided) ? lim : limit;
  return list;
}

void mvs_graves_entity_list_destroy(MVSGravesEntityList *list) {
  if (!list)
    return;
  if (list->current_entity_count) {
    for (msize_t i = 0; i < list->current_entity_count; i++) {
      mvs_graves_entity_utils_destroy_entity(
          *(MVSEntity **)mvs_dynamic_listl_ref_of_unsafe(list->entity_list, i));
    }
  }
  mvs_dynamic_listl_destroy(list->entity_list);
  free(list);
}

mbool_t mvs_graves_entity_list_add_entity(MVSGravesEntityList *list,
                                          MVSEntity *ent) {
  /*
   * The way the list is used by Graves is such that Graves will verify that the
   * list can store new entities and then and only then will the list add new
   * entities. Just adding a new entity doesn't imply that it is active
   * */
  msize_t ID = list->current_entity_count;
  if (mvs_dynamic_listl_push(list->entity_list, &ent) != MRES_SUCCESS) {
    mvs_log_err(
        "Operation failed: add_entity_to_list: [AEC=%zu,TEC=%zu,CEC=%zu]",
        list->active_entity_count, list->total_entity_count,
        list->current_entity_count);
    return mfalse;
  }
  list->current_entity_count++;
  ent->identity.ID = ID;
  if (list->current_entity_count >= list->total_entity_count)
    list->total_entity_count = mvs_dynamic_listl_cap_unsafe(list->entity_list);
  return mtrue;
}

MVSEntity *mvs_graves_entity_list_find_free_entity(MVSGravesEntityList *list) {
  MVSEntity *ent = NULL;
  for (msize_t i = 0; i < list->current_entity_count; i++) {
    ent = *(MVSEntity **)mvs_dynamic_listl_ref_of_unsafe(list->entity_list, i);
    if (atomic_load_explicit(&ent->state, memory_order_relaxed) ==
        MENTITY_STOPPED) {
      atomic_store_explicit(&ent->state, MENTITY_UNKNOWN, memory_order_release);
      break;
    }
    ent = NULL;
  }
  return ent;
}

MVSEntity *mvs_graves_entity_list_get_entity(MVSGravesEntityList *list,
                                             MVSEntityIdentity *iden) {
  if (iden->ID >= list->current_entity_count)
    return NULL;
  MVSEntity *ent = *(MVSEntity **)mvs_dynamic_listl_ref_of_unsafe(
      list->entity_list, iden->ID);
  if (ent->identity.UID != iden->UID)
    return NULL;
  return ent;
}

MVSEntity *mvs_graves_entity_list_get_entity_by_ID(MVSGravesEntityList *list,
                                                   msize_t ID) {
  if (ID >= list->current_entity_count)
    return NULL;
  MVSEntity *ent =
      *(MVSEntity **)mvs_dynamic_listl_ref_of_unsafe(list->entity_list, ID);
  return ent;
}

void mvs_graves_entity_list_register_active_entity(MVSGravesEntityList *list,
                                                   MVSEntity *ent) {
  msize_t UID = atomic_fetch_add(&list->total_entities_ever_created, 1);
  ent->identity.UID = UID;
  atomic_fetch_add(&list->active_entity_count, 1);
}

void mvs_graves_entity_list_unregister_active_entity(MVSGravesEntityList *list,
                                                     MVSEntity *ent) {
  atomic_fetch_sub(&list->active_entity_count, 1);
  ent->identity.UID = (mqword_t)-1;
  mvs_graves_entity_utils_clear_local_list(ent);
}
