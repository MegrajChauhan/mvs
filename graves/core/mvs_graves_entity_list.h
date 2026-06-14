#ifndef _MVS_GRAVES_ENTITY_LIST_
#define _MVS_GRAVES_ENTITY_LIST_

#include <mvs_graves_entity_utils.h>
#include <mvs_entity.h>
#include <mvs_list.h>
#include <mvs_logger.h>
#include <mvs_types.h>

typedef struct MVSGravesEntityList MVSGravesEntityList;

struct MVSGravesEntityList {
  atm_msize_t active_entity_count; // just accounting information
  msize_t total_entity_count;      // The total number of entities that the list can have
  msize_t current_entity_count;    // How many entities the list currently has, active and dead? Used to provide ID
  atm_msize_t total_entities_ever_created; // accounting information, used for UID

  MVSDynamicListLinear *entity_list;

  msize_t active_entity_limit; // Set a limit on how many entities can be
                               // created at a given time The user cannot have
                               // many entities created at a given time but only
                               // a few are active at once
};

MVSGravesEntityList *mvs_graves_entity_list_create(msize_t init, msize_t limit);

void mvs_graves_entity_list_destroy(MVSGravesEntityList *list);

mbool_t mvs_graves_entity_list_add_entity(MVSGravesEntityList *list,
                                          MVSEntity *ent);

MVSEntity *mvs_graves_entity_list_find_free_entity(MVSGravesEntityList *list);

MVSEntity *mvs_graves_entity_list_get_entity(MVSGravesEntityList *list, MVSEntityIdentity *iden);

MVSEntity *mvs_graves_entity_list_get_entity_by_ID(MVSGravesEntityList *list, msize_t ID);

void mvs_graves_entity_list_register_active_entity(MVSGravesEntityList *list, MVSEntity *ent);

void mvs_graves_entity_list_unregister_active_entity(MVSGravesEntityList *list, MVSEntity *ent);

_MVS_ATTR_ALWAYS_INLINE_ mbool_t
mvs_graves_entity_list_has_free_entity(MVSGravesEntityList *list) {
  return (list->active_entity_count < list->current_entity_count) ? mtrue
                                                                : mfalse;
}

_MVS_ATTR_ALWAYS_INLINE_ mbool_t
mvs_graves_entity_list_reached_limit(MVSGravesEntityList *list) {
  return (list->active_entity_limit <= list->active_entity_count) ? mtrue
                                                                  : mfalse;
}

_MVS_ATTR_ALWAYS_INLINE_ mbool_t
mvs_graves_entity_list_validate_identity(MVSGravesEntityList *list, MVSEntityIdentity *iden) {
  return (list->current_entity_count > iden->ID) ? mtrue : mfalse;
}

_MVS_ATTR_ALWAYS_INLINE_ msize_t 
mvs_graves_entity_list_get_active_entity_count(MVSGravesEntityList *list) {
  return atomic_load_explicit(&list->active_entity_count, memory_order_relaxed);
}

_MVS_ATTR_ALWAYS_INLINE_ msize_t 
mvs_graves_entity_list_get_current_entity_count(MVSGravesEntityList *list) {
		return list->current_entity_count;
}

#endif
