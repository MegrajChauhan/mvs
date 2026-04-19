#ifndef _MVS_OPERATIONS_
#define _MVS_OPERATIONS_

typedef enum mResult_t mResult_t;

enum mResult_t {
  MRES_SUCCESS,      // The operation was successful
  MRES_INVALID_ARGS, // operation failed because invalid arguments were provided
  MRES_SYS_FAILURE,  // operation failed because glibc or syscall failed(check
                     // ERRNO)
  MRES_COULDNT_COMPLETE, // operation didn't fail but it couldn't be
                         // completed(due to source specific reason)
  MRES_RESOURCE_SHARED,  // The operation failed because the resource is shared and the requestor is not the sole owner of the resource
  MRES_RESOURCE_STATE_INVALID, // the requested operand resources' state was invalid
  MRES_RESOURCE_TYPE_INVALID, // the used operation doesn't take the provided type of operand 
  MRES_RESOURCE_NOT_CONFIGURED, // The used operand was not configured for the requested operation
  MRES_RESOURCE_SIZE_LIMITED,  // the resource's size is too small for the request
  // MRES_RESOURCE_NOT_EXISTS,    // operation failed because something was
  // accessed but it doesn't exist MRES_OPER_NOT_AVAI, // operation failed
  // because the requested operation isn't available for some reason
  // MRES_OPER_NOT_PERM, // operation failed because the requested operation
  // isn't permitted MRES_RESOURCE_LIM_REACHED, // operation failed because
  // resource limit was reached MRES_CONFIGURATION_INVALID, // operation failed
  // because the arguments were not configured properly MRES_INVALID_ACCESS, //
  // operation failed because an access was invalid MRES_OPER_UNRECOGNIZED, //
  // operation failed because the requested operation isn't supported at all
  // MRES_INVALID_INTERFACE, // operation failed because the provided interface
  // isn't the correct interface
};

#endif
