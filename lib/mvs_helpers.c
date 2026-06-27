#include <mvs_helpers.h>

msize_t mvs_align_value(msize_t val, msize_t align_to) {
  return val + ((val % align_to == 0) ? 0 : align_to - (val % align_to));
}
