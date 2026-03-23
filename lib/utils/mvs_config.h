#ifndef _MVS_CONFIG_
#define _MVS_CONFIG_

/*
 * We only support 64-bit systems
 */
#if __SIZEOF_VOID__ == 4
#error Merry Virtual System Only supports 64-bit systems
#endif

#define _MVS_HOST_ARCH_LITTLE_ENDIAN_ 0x00
#define _MVS_HOST_ARCH_BIG_ENDIAN_ 0x01

#define _MVS_OPTIMIZE_

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define _MVS_HOST_ARCH_BYTE_ORDER_ _MVS_HOST_ARCH_LITTLE_ENDIAN_
#else
#define _MVS_HOST_ARCH_BYTE_ORDER_ _MVS_HOST_ARCH_BIG_ENDIAN_
#endif

/*
 * Merry follows the same endianness as the Host
 */
#define _MVS_HOST_ARCH_ENDIANNESS_ _MVS_HOST_ARCH_BYTE_ORDER_

#if defined(__amd64) || defined(__amd64__)
#define _MVS_HOST_CPU_AMD_
#define _MVS_HOST_ARCH_X86_
#endif

#if defined(__x86_64)
#define _MVS_HOST_CPU_X86_
#endif

// Host OS detection
#if defined(__linux) || defined(__linux__) || defined(__gnu_linux__)
#define _MVS_HOST_PLAT_LINUX_
#define _USE_LINUX_
#ifndef __USE_MISC // for STDLIB
#define __USE_MISC
#endif
#endif

#if __SIZEOF_LONG__ == __SIZEOF_LONG_LONG__
#define _MVS_TYPE_LONG_ long
#else
#define _MVS_TYPE_LONG_ long long
#endif

#endif
