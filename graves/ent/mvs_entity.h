#ifndef _MVS_ENTITY_
#define _MVS_ENTITY_

#include <mvs_entity_defs.h>
#include <mvs_list.h>
#include <mvs_tools.h>
#include <mvs_types.h>

typedef struct MVSEntity MVSEntity;
typedef struct MVSEntityIdentity MVSEntityIdentity;
typedef struct MVSEntityState MVSEntityState;
typedef union MVSEntityConfig MVSEntityConfig;
typedef union MVSEntityProperties MVSEntityProperties;

struct MVSEntityIdentity {
  mid_t ID;   // Entity specific ID
  muid_t UID; // System provided unique ID
};

struct MVSEntityState {
  _Atomic mEntityState_t state;
};

union MVSEntityConfig {
  /*
   * This represents the configuration that the entity starts in
   */
  struct {
    mqword_t can_spawn_entity : 1;  // Can the entity spawn more entity?
    mqword_t local_ent_list_en : 1; // Enable local entity list?
    mqword_t local_ent_list_size_lim
        : 1; // Set limit to the number of entities that the local list can
             // store Effectively limiting the number of entities one entity can
             // interact with
    mqword_t resb : 61;
  } _;
  mqword_t config;
};

union MVSEntityProperties {
  /*
   * This represents the properties that determine the behavior of the entity in
   * the future While config cannot be changed once set, properties can be
   * changed
   */
  struct {
    mqword_t resb;
  } _;
  mqword_t props;
};

/*
 * This is what Graves will use to represent a single instance of an entity
 */
struct MVSEntity {
  mEntity_t type;
  msize_t EID;
  MVSEntityIdentity identity;
  MVSEntityState state;
  MVSEntityConfig config;
  MVSEntityProperties properties;
  mptr_t entity_repr;

  msize_t entity_local_list_size_lim;
  msize_t entity_local_list_tracks;
  msize_t entity_local_list_history;
  MVSDynamicListLinear *entity_local_list;
};

#endif
