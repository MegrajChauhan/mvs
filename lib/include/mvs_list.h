#ifndef _MVS_LIST_
#define _MVS_LIST_

/*
 * We are gonna have:
 * 1. Static list
 * 2. lock free static list here
 * 3. Dynamic list(not lock free)
 * */

#include <mvs_logger.h>
#include <mvs_results.h>
#include <mvs_tools.h>
#include <mvs_types.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h> // memcpy

/*------------------STATIC LIST---------------------*/
typedef struct MVSStaticList MVSStaticList;

struct MVSStaticList {
  msize_t cap;
  msize_t curr_ind;
  msize_t elem_len;
  mptr_t buf;
};

mResult_t mvs_static_list_create(msize_t cap, msize_t elem_len,
                                 MVSStaticList **lst);
mResult_t mvs_static_list_destroy(MVSStaticList *lst);

/*
 * It should be a pointer like:
 * For 'int' list:
 * int a = 90; // to store 90
 * mvs_static_list_push(lst, &a);
 * The list will copy the value and store it
 * If you want to store a pointer(valid and allocated), just use pointers
 */
mResult_t mvs_static_list_push(MVSStaticList *lst, mptr_t elem);
/*
 * The same logic applies to mvs_static_list_pop as it applied to
 * mvs_static_list_push Here, the function will dereference the pointer to store
 * the popped element instead It can be NULL to indicate that the popped element
 * is not desired
 */
mResult_t mvs_static_list_pop(MVSStaticList *lst, mptr_t elem);
mResult_t mvs_static_list_at(MVSStaticList *lst, mptr_t elem, msize_t ind);
mResult_t mvs_static_list_resize(MVSStaticList *lst, msize_t resize_factor);
/*
 * The reason that some functions(like below) don't use mResult_t is because for
 * functions such as this that only access a property without doing major
 * changes or accesses are okay to not use mResult_t(as the function can't even
 * fail in more than 1 way) But why mvs_static_list_destroy used mResult_t even
 * with just one way to fail? That's because it performed the radical operation
 * of destroying the resource itself which requires observation
 */
msize_t mvs_static_list_size(MVSStaticList *lst);
/*
 * Here, the function returns a reference to the element.
 * The pointer provided as 'elem' should be a reference to a pointer that points
 * to the stored element type
 */
mResult_t mvs_static_list_ref_of(MVSStaticList *lst, mptr_t elem, msize_t ind);

/*
 * the mvs_*_list_index_of function takes elem pointer which must be a pointer
 * returned by mvs_*_list_ref_of and not a pointer to a pointer
 */
msize_t mvs_static_list_index_of(MVSStaticList *lst, mptr_t elem);

/*-----------------END STATIC LIST--------------------*/

/*------------------LF STATIC LIST---------------------*/
/*
 * The only use of this list is if there is only one owner that pops elements
 * whereas any number of owners may push. As there is no way to tell the state
 * of the list without either push or pop operation, one should decide who will
 * be the producer and who will be the consumer
 */
typedef struct MVSLFList MVSLFList;
struct MVSLFList {
  msize_t cap;
  msize_t curr_ind;
  msize_t elem_len;
  mptr_t buf;
};

mResult_t mvs_lf_list_create(msize_t cap, msize_t elem_len, MVSLFList **lst);
mResult_t mvs_lf_list_destroy(MVSLFList *lst);
mResult_t mvs_lf_list_push(MVSLFList *lst, mptr_t elem);
mResult_t mvs_lf_list_pop(MVSLFList *lst, mptr_t elem);

/*-----------------END LF STATIC LIST--------------------*/

/*------------------DYNAMIC LIST---------------------*/
typedef struct MVSDynamicList MVSDynamicList;
struct MVSDynamicList {
  msize_t cap;
  msize_t curr_ind;
  msize_t elem_len;
  mptr_t buf;
};
mResult_t mvs_dynamic_list_create(msize_t cap, msize_t elem_len,
                                  MVSDynamicList **lst);
mResult_t mvs_dynamic_list_destroy(MVSDynamicList *lst);
mResult_t mvs_dynamic_list_push(MVSDynamicList *lst, mptr_t elem);
mResult_t mvs_dynamic_list_pop(MVSDynamicList *lst, mptr_t elem);
mResult_t mvs_dynamic_list_at(MVSDynamicList *lst, mptr_t elem, msize_t ind);
mResult_t mvs_dynamic_list_resize(MVSDynamicList *lst, msize_t resize_factor);
msize_t mvs_dynamic_list_size(MVSDynamicList *lst);
mResult_t mvs_dynamic_list_ref_of(MVSDynamicList *lst, mptr_t *elem,
                                  msize_t ind);
msize_t mvs_dynamic_list_index_of(MVSDynamicList *lst, mptr_t elem);

/*-----------------END DYNAMIC LIST--------------------*/

#endif
