#ifndef _MVS_GRAVES_ENTITY_UTILS_
#define _MVS_GRAVES_ENTITY_UTILS_

#include <api_request_response.h>
#include <api_entity.h>
#include <api_graves.h>
#include <mvs_entity.h>
#include <mvs_entity_registry.h>
#include <mvs_logger.h>
#include <mvs_request_response_conversion.h>
#include <mvs_types.h>
#include <stdlib.h>

#define _MVS_MFUNC_ENTITY_CONTEXT_INIT_(iden, _argc, _argv, _slist) (EntityContext){.self=(iden),.argc=(_argc), .argv=(_argv), .slist=(_slist)}

MVSEntity *mvs_graves_entity_utils_create_entity();

void mvs_graves_entity_utils_destroy_entity(MVSEntity *ent);

mbool_t mvs_graves_entity_utils_config_valid(msize_t conf);

mbool_t mvs_graves_entity_utils_properties_valid(msize_t props);

mbool_t mvs_graves_entity_utils_init_entity(MVSEntity *ent, EntityContext *ctx,
                                            msize_t EID, mEntity_t type,
                                            msize_t conf, msize_t props,
                                            mqword_t in_conf,
                                            APIRequestResponse *resp);

mbool_t mvs_graves_entity_utils_init_entity_hotpath(MVSEntity *ent,
                                                    EntityContext *ctx,
                                                    msize_t EID, mEntity_t type,
                                                    msize_t conf, msize_t props,
                                                    mqword_t in_conf);

mbool_t mvs_graves_entity_utils_prepare_entity(MVSEntity *ent,
                                               APIRequestResponse *resp,
                                               msize_t local_ent_list_size_lim);

mbool_t
mvs_graves_entity_utils_prepare_entity_hotpath(MVSEntity *ent,
                                               msize_t local_ent_list_size_lim);

mbool_t mvs_graves_entity_utils_check_local_list_enable(MVSEntity *ent);

mbool_t mvs_graves_entity_utils_check_local_list_limit_set(MVSEntity *ent);

mbool_t mvs_graves_entity_utils_check_local_list_full(MVSEntity *ent);

mbool_t mvs_graves_entity_utils_add_to_local_list(MVSEntity *ent, MVSEntity *to_add);

mbool_t mvs_graves_entity_utils_clear_local_list(MVSEntity *ent);

mbool_t mvs_graves_entity_utils_probe_local_list_position(MVSEntity *ent, MVSEntityIdentity *res, msize_t pos);

#endif
