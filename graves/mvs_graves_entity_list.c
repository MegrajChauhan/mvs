#include <mvs_graves_entity_list.h>

MVSGravesEntityList* mvs_graves_entity_list_create(msize_t limit) {
		MVSGravesEntityList *list = (MVSGravesEntityList *)malloc(sizeof(MVSGravesEntityList));
		if (!list) {
				mvs_log_err("Failed to initialize internal entity list");
				return NULL;
		}

		mbool_t limit_provided = (limit == (mqword_t)(-1))? mtrue: mfalse;
		msize_t lim = (limit_provided)? limit: 100;
		if(mvs_dynamic_listl_create(&list->list, lim, sizeof(MVSEntity *)) != MRES_SUCCESS) {
				mvs_log_err("Failed to initialize internal entity list");
				free(list);
				return NULL;
		}

		list->active_entity_count = 0;
		list->total_entity_count = 0;
		list->active_entity_limit = (limit_provided)? lim: limit;
		return list;
}

void mvs_graves_entity_list_destroy(MVSGravesEntityList *list) {
		mvs_dynamic_listl_destroy(list->list);
		free(list);
}

mbool_t mvs_graves_entity_list_add_entity(MVSGravesEntityList *list, MVSEntity *ent) {
		if (mvs_dynamic_listl_push(list->list, &ent) != MRES_SUCCESS) {
				mvs_log_err("Operation failed: add_entity_to_list: [AEC=%zu,TEC=%zu,AEL=%zu]", list->active_entity_count, list->total_entity_count, list->active_entity_limit);
				return mfalse;
		}
		list->total_entity_count++;
		return mtrue;
}

MVSGravesEntityList* mvs_graves_entity_list_find_free_entity(MVSGravesEntityList *list) {
		MVSEntity *ent = NULL;
		for (msize_t i = 0; i < list->total_entity_count; i++) {
				ent = *(MVSEntity**)mvs_dynamic_listl_ref_of_unsafe(list->list, i);
				if (!ent->entity_repr)
						break;
				ent = NULL;
		}
		return ent;
}
