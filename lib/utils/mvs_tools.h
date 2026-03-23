#ifndef _MVS_DEFS_
#define _MVS_DEFS_

#ifndef surelyT
#define surelyT(x)                                                             \
  __builtin_expect(                                                            \
      !!(x),                                                                   \
      1) // tell the compiler that the expression x is most likely to be true
#define surelyF(x)                                                             \
  __builtin_expect(                                                            \
      !!(x),                                                                   \
      0) // tell the compiler that the expression x is most likely to be false
#endif

#if defined(_MVS_MODE_OPTIMIZE_)
#define _MVS_ATTR_ALWAYS_INLINE_ inline __attribute__((always_inline))
#else
#define _MVS_ATTR_ALWAYS_INLINE_ static inline
#endif

#define _MVS_ATTR_NO_DISCARD_ __attribute__((nodiscard))
#define _MVS_ATTR_NO_THROW_ __attribute__((no_throw))
#define _MVS_ATTR_NO_RETURN_ __attribute__((no_return))
#define _MVS_ATTR_NO_NULL_ __attribute__((nonnull))
#define _MVS_ATTR_ALIAS_(name) __attribute__((alias(#name)))
#define _MVS_ATTR_CONSTRUCTOR_ __attribute__((constructor))

#define _MVS_ATTR_INTERNAL_                                                    \
  static // for a variable or a function that is localized to a module only
#define _MVS_ATTR_LOCAL_ static // any static variable inside a function
#define _MVS_ATTR_EXTERNAL_ extern

#define _stringify(x) #x
#define _glue(x, y) x##y
#define _toggle(x) (~x) >> 63
#define _MVS_MFUNC_STRINGIFY_(to_str) _stringify(to_str)
#define _MVS_MFUNC_CONCAT_(x, y) _glue(x, y)
#define _MVS_MFUNC_TOGGLE_(x) _toggle(x) // toggle a boolean value
#define _MVS_MFUNC_TO_BOOL_(x) !!(x)     // convert x to bool

#endif
