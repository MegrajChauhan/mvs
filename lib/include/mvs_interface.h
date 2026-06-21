#ifndef _MVS_INTERFACE_
#define _MVS_INTERFACE_

#include <mvs_helpers.h>
#include <mvs_interface_flags.h>
#include <mvs_interface_types.h>
#include <mvs_platform.h>
#include <mvs_results.h>
#include <mvs_types.h>
#include <mvs_protectors.h>
#include <stdlib.h>

typedef struct MVSInterface MVSInterface;

struct MVSInterface {
  mInterface_t interface;
  mmutex_t lock;

  mbool_t configured; // Was the interface configured?

  mqword_t config; // This configuration cannot be changed once set during
                       // intialization
  atm_mqword_t
      flags; // These are mutable flags to manipulate the interface's behavior
  atm_mqword_t state; // The state of the interface

  atm_msize_t owner_count;

  union {
    struct {
      mfd_t fd;
      struct {
        msize_t read : 1;
        msize_t write : 1;
        msize_t append : 1;
        msize_t new : 1; // Was the file created?
        // msize_t trunc: 1;  // not yet implemented
        msize_t resb : 59;
      } flags;
    } file;
    struct {
      mmap_t addr_space;
      msize_t addr_space_len;
      msize_t offset;
      msize_t align_param;
      MVSInterface *backing; // Is there a backing file for this mapped memory?

      // if backing == NULL than, the mapping is anonymous
      struct {
        msize_t can_read : 1;
        msize_t can_write : 1;
        msize_t can_exec : 1; // This is ignored really

        msize_t shared : 1;  // The updates on the address space is shared with
                             // other processes sharing the same memory region
        msize_t private : 1; // Opposite of shared
        msize_t sync : 1; // This causes the changes made on the mapped memory
                          // to be immediately reflected on the file(the
                          // operation is synchronous)

        msize_t resb : 58;
      } flags;
    } mapped_mem;
    struct {
      mdlentry_t entry;
    } dynamic_lib;
  };
};

/*
 * This just creates the interface. It doesn't have any type just yet and can be
 * turned into any type as required.
 */
mResult_t mvs_interface_create(MVSInterface **interface);

/* if conf = 0 than the interface will be initialized with default
 * configurations: SHAREABLE = false
 */
mResult_t mvs_interface_configure(MVSInterface *interface, mqword_t conf);

/*
 * For operations that involve changing the internal state of the interface or 
 * freeing resources, the owners must lock the interface.
 * */
mResult_t mvs_interface_lock(MVSInterface *interface);

mResult_t mvs_interface_unlock(MVSInterface *interface);

/*
 * This doesn't destroy the interface but frees it to be usable again
 */
mResult_t mvs_interface_free(MVSInterface *interface);

/*
 * This frees the resources of the interface completely
 */
mResult_t mvs_interface_destroy(MVSInterface *interface);

/*
 * This initializes the interface with just a type. The interface hasn't
 * necessarily been fully initialized yet but it has been designated a type for
 * now. The initialization may follow later on. If the initialization fails
 * later on than the interface is made 'free' immediately.
 */
mResult_t mvs_interface_init(MVSInterface *interface, mInterface_t type);

/*
 * Setter function setting the flags of the interface
 */
mResult_t mvs_interface_set_flags(MVSInterface *interface, mqword_t flags);

/*
 * Sharing of the interface happens by simply sharing the same interface. A new
 * instance isn't allocated and we keep track of the owners using the
 * owner_count field. This is acceptable as every entity using the interface is
 * a part of the same process
 */
mResult_t mvs_interface_share(MVSInterface **source, MVSInterface **dest);

/*
 * Once the shared interface has done its work, it can be disowned
 */
mResult_t mvs_interface_disown(MVSInterface *interface);

/*
 * Check if the interface can be freed. Since the owner has to be careful not to free a 
 * shared resource while others might be using it, this is a must.
 * */
mResult_t mvs_interface_check_freeable(MVSInterface *interface);

#endif
