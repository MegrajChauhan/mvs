#ifndef _MVS_TYPES_
#define _MVS_TYPES_

#include <mvs_config.h>
#include <mvs_results.h>

// boolean types
#define mtrue 1
#define mfalse 0

typedef unsigned char mbool_t; // bool

// these are the memory data types
typedef unsigned char mbyte_t;
typedef unsigned short mword_t;
typedef unsigned int mdword_t;
typedef unsigned _MVS_TYPE_LONG_ mqword_t;

typedef char msbyte_t;
typedef short msword_t;
typedef int msdword_t;
typedef _MVS_TYPE_LONG_ msqword_t;

// some extra definitions
typedef unsigned _MVS_TYPE_LONG_ maddress_t;
typedef unsigned _MVS_TYPE_LONG_ msize_t;
typedef msize_t mid_t;   // ID of something
typedef msize_t muid_t;  // UID of something
typedef msize_t mguid_t; // Group ID of something

typedef _Atomic msize_t atm_msize_t;
typedef _Atomic mbool_t atm_mbool_t;
typedef _Atomic mqword_t atm_mqword_t;
typedef _Atomic mbyte_t atm_mbyte_t;

// pointers for memory types
typedef mbyte_t *mbptr_t;
typedef mword_t *mwptr_t;
typedef mdword_t *mdptr_t;
typedef mqword_t *mqptr_t;

// redifinitions of some C types
typedef char *mstr_t;
typedef const char *mcstr_t;
typedef void *mptr_t;

#if defined(_USE_LINUX_)
#define mthreadRet_t mptr_t
#else
#endif

typedef mthreadRet_t (*mthexec_t)(mptr_t);
typedef mResult_t (*mhcleanfunc_t)(mptr_t);
typedef msize_t (*mhhashfunc_t)(msize_t);
typedef mbool_t (*mhkeycmpfunc_t)(mptr_t, mptr_t);

#endif
