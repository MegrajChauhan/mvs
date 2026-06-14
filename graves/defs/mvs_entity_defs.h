#ifndef _MVS_ENTITY_DEFS_
#define _MVS_ENTITY_DEFS_

typedef enum mEntity_t mEntity_t;
typedef enum mEntityState_t mEntityState_t;

enum mEntity_t {
  MENTITY_LOCAL,    // or built-in entity
  MENTITY_EXTERNAL, // or user-defined entity
};

enum mEntityState_t {
  MENTITY_INIT,     // Entity Just Initialized
  MENTITY_READY_TO_RUN, // Entity prepared to run
  MENTITY_STOPPED,  // Entity finished its work
  MENTITY_RUNNING, // Entity is running
  MENTITY_BUSY, // Entity is busy
  MENTITY_UNKNOWN, // Intermediate state used internally
};

#define MVS_CONF_ENTITY_CAN_SPAWN_ENTITY (1ULL << 0)
#define MVS_CONF_ENTITY_LOCAL_ENTITY_LIST_ENABLE (1ULL << 1)
#define MVS_CONF_ENTITY_ENTITY_TRACKING_LIM (1ULL << 2)

#endif
