#ifndef _MVS_GRAVES_ENTITY_UTILS_
#define _MVS_GRAVES_ENTITY_UTILS_

#include <mvs_types.h>
#include <mvs_entity_registry.h>
#include <mvs_entity.h>
#include <api_entity_registration.h>
#include <api_request_response.h>
#include <mvs_logger.h>
#include <mvs_request_response_conversion.h>
#include <stdlib.h>

MVSEntity* mvs_graves_entity_utils_create_entity(); 

void mvs_graves_entity_utils_destroy_entity(MVSEntity *ent);

mbool_t mvs_graves_entity_utils_config_valid(msize_t conf);

mbool_t mvs_graves_entity_utils_properties_valid(msize_t props);

mbool_t mvs_graves_entity_utils_init_entity(MVSEntity *ent, EntityContext *ctx, msize_t EID, mEntity_t type, 
				     msize_t conf, msize_t props, mqword_t in_conf, APIRequestResponse *resp);

mbool_t mvs_graves_entity_utils_prepare_entity(MVSEntity *ent, APIRequestResponse *resp, msize_t local_ent_list_size_lim);

#endif
