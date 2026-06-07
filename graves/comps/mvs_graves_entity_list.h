#ifndef _MVS_GRAVES_ENTITY_LIST_
#define _MVS_GRAVES_ENTITY_LIST_

#include <mvs_entity.h>
#include <mvs_list.h>
#include <mvs_logger.h>
#include <mvs_types.h>

typedef struct MVSGravesEntityList MVSGravesEntityList;

struct MVSGravesEntityList {
  atm_msize_t active_entity_count;
  msize_t total_entity_count;

  MVSDynamicListLinear *list;

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

_MVS_ATTR_ALWAYS_INLINE_ MVSDynamicListLinear *
mvs_graves_entity_list_get_entity_list(MVSGravesEntityList *list) {
  return list->list;
}

_MVS_ATTR_ALWAYS_INLINE_ MVSEntity *
mvs_graves_entity_list_get_entity(MVSGravesEntityList *list, msize_t ID) {
  return *(MVSEntity **)mvs_dynamic_listl_ref_of_unsafe(list->list, ID);
}

_MVS_ATTR_ALWAYS_INLINE_ mbool_t
mvs_graves_entity_list_has_free_entity(MVSGravesEntityList *list) {
  return (list->active_entity_count < list->total_entity_count) ? mtrue
                                                                : mfalse;
}

_MVS_ATTR_ALWAYS_INLINE_ mbool_t
mvs_graves_entity_list_reached_limit(MVSGravesEntityList *list) {
  return (list->active_entity_limit >= list->active_entity_count) ? mtrue
                                                                  : mfalse;
}

_MVS_ATTR_ALWAYS_INLINE_ void
mvs_graves_entity_list_register_active_entity(MVSGravesEntityList *list) {
  list->active_entity_count++;
}

_MVS_ATTR_ALWAYS_INLINE_ void
mvs_graves_entity_list_unregister_active_entity(MVSGravesEntityList *list) {
  list->active_entity_count--;
}

_MVS_ATTR_ALWAYS_INLINE_ mbool_t
mvs_graves_entity_list_non_empty(MVSGravesEntityList *list) {
  return (list && list->active_entity_count > 0) ? mtrue : mfalse;
}

_MVS_ATTR_ALWAYS_INLINE_ mbool_t
mvs_graves_entity_list_validate_ID(MVSGravesEntityList *list, msize_t ID) {
  return (list->total_entity_count > ID) ? mtrue : mfalse;
}

_MVS_ATTR_ALWAYS_INLINE_ msize_t 
mvs_graves_entity_list_get_active_entity_count(MVSGravesEntityList *list) {
  return list->active_entity_count;
}

#endif
