#ifndef _MVS_SLIST_TOKEN_
#define _MVS_SLIST_TOKEN_

#include <mvs_helpers.h>
#include <mvs_slist_defs.h>

typedef enum mSlistToken_t mSlistToken_t;
typedef struct MVSSlistToken MVSSlistToken;

enum mSlistToken_t {
  MVS_SLIST_TOK_EOF,
  MVS_SLIST_TOK_ERR,
  MVS_SLIST_TOK_EOL,
  MVS_SLIST_TOK_OPEN_CURLY,
  MVS_SLIST_TOK_CLOSE_CURLY,
  MVS_SLIST_TOK_OPEN_BRAC,
  MVS_SLIST_TOK_CLOSE_BRAC,
  MVS_SLIST_TOK_COMMA,
  MVS_SLIST_TOK_COLON,
  MVS_SLIST_TOK_IDEN, // commands are also identifiers for the lexer
  MVS_SLIST_TOK_NUM,
  MVS_SLIST_TOK_EQUALS,
};

struct MVSSlistToken {
  mSlistToken_t type;
  msize_t line, col;
  union {
    MVSStrSlice iden;
    MVSStrSlice num;
  };
};

#endif
