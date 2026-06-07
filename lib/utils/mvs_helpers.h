#ifndef _MVS_HELPERS_
#define _MVS_HELPERS_

#include <ctype.h>
#include <mvs_config.h>
#include <mvs_platform.h>
#include <mvs_results.h>
#include <mvs_types.h>
#include <string.h>

#define _MVS_MFUNC_SIGN_EXTEND8_(val)                                          \
  do {                                                                         \
    if ((val >> 7) == 1)                                                       \
      val |= 0xFFFFFFFFFFFFFF00;                                               \
  } while (0)
#define _MVS_MFUNC_SIGN_EXTEND16_(val)                                         \
  do {                                                                         \
    if ((val >> 15) == 1)                                                      \
      val |= 0xFFFFFFFFFFFF0000;                                               \
  } while (0)
#define _MVS_MFUNC_SIGN_EXTEND32_(val)                                         \
  do {                                                                         \
    if ((val >> 31) == 1)                                                      \
      val |= 0xFFFFFFFFFF000000;                                               \
  } while (0)

#define _MVS_MFUNC_BIT_GROUP_(name, len) unsigned name : len
#define _MVS_MFUNC_ISUPPER_(ch) ((ch) >= 'A' && (ch) <= 'Z')
#define _MVS_MFUNC_ISLOWER_(ch) ((ch) >= 'a' && (ch) <= 'z')
#define _MVS_MFUNC_ISALPHA_(ch)                                                \
  (_MVS_MFUNC_ISLOWER_(ch) || _MVS_MFUNC_ISUPPER_(ch))
#define _MVS_MFUNC_ISNUM_(ch) (((ch) >= '0' && (ch) <= '9'))
#define _MVS_MFUNC_ISALNUM_(ch)                                                \
  (_MVS_MFUNC_ISALPHA_(ch) || _MVS_MFUNC_ISNUM_(ch))
#define _MVS_MFUNC_ISSPACE_(ch) isspace(ch)

typedef union MVSPtrToQword MVSPtrToQword;
typedef union MVSHostMemLayout MVSHostMemLayout;
typedef union MVSFloatToDword MVSFloatToDword;
typedef union MVSDoubleToQword MVSDoubleToQword;
typedef struct MVSStrSlice MVSStrSlice;

union MVSPtrToQword {
  mptr_t ptr;
  mqword_t qword;
};

union MVSHostMemLayout {

#if _MVS_ENDIANNESS_ == _MVS_LITTLE_ENDIAN_
  struct {
    mbyte_t b7;
    mbyte_t b6;
    mbyte_t b5;
    mbyte_t b4;
    mbyte_t b3;
    mbyte_t b2;
    mbyte_t b1;
    mbyte_t b0;
  } bytes;

  struct {
    mword_t w3;
    mword_t w2;
    mword_t w1;
    mword_t w0;
  } half_half_words;

  struct {
    mdword_t w1;
    mdword_t w0;
  } half_words;
#else
  struct {
    mbyte_t b0;
    mbyte_t b1;
    mbyte_t b2;
    mbyte_t b4;
    mbyte_t b5;
    mbyte_t b3;
    mbyte_t b6;
    mbyte_t b7;
  } bytes;

  struct {
    mword_t w0;
    mword_t w1;
    mword_t w2;
    mword_t w3;
  } half_half_words;

  struct {
    mdword_t w0;
    mdword_t w1;
  } half_words;
#endif
  mqword_t whole_word;
};

union MVSFloatToDword {
  float fl_val;
  mdword_t d_val;
};

union MVSDoubleToQword {
  double d_val;
  mqword_t q_val;
};

struct MVSStrSlice {
  mstr_t st;
  msize_t len;
};

// void MVS_LITTLE_ENDIAN_to_BIG_ENDIAN(MVSHostMemLayout *le);

// mresult_t MVS_open_pipe(mdataline_t *rline, mdataline_t *wline);

// mbool_t MVS_is_path_a_directory(mstr_t path);

// msize_t MVS_align_value(msize_t val, msize_t align_to);

#endif
