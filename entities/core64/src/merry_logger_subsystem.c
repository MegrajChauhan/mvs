#include <merry_logger_subsystem.h>

_MVS_ATTR_THREAD_LOCAL_ msize_t signature = 0;

void merry_set_signature(msize_t sig) {
	signature = sig;
}
