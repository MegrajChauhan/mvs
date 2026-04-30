#ifndef _MVS_ENTITY_DEFS_
#define _MVS_ENTITY_DEFS_

typedef enum mEntity_t mEntity_t;
typedef enum mEntityState_t mEntityState_t;

enum mEntity_t {
	MENTITY_LOCAL,    // or built-in entity
	MENTITY_EXTERNAL, // or user-defined entity
};

enum mEntityState_t {
	MENTITY_STOPPED,
	MENTITY_RUNNING,
	MENTITY_BUSY,
};

#endif
