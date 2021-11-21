#pragma once

/** Detect the Operating System **/
#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__)
#define METHAN_OS_WINDOWS
#define METHAN_OS_NAME                 "Microsoft Windows"
#elif defined(macintosh) || defined(Macintosh) || (defined(__APPLE__) && defined(__MACH__))
#define METHAN_OS_MACOS
#define METHAN_OS_NAME                 "Mac OS"
#define METHAN_OS_UNIX_LIKE
#elif defined(linux) || defined(__linux)
#define METHAN_OS_LINUX
#define METHAN_OS_NAME                 "Linux"
#define METHAN_OS_UNIX_LIKE
#elif defined(unix) || defined(__unix) || defined(_XOPEN_SOURCE) || defined(_POSIX_SOURCE)
#define METHAN_OS_UNIX
#define METHAN_OS_NAME                 "Unix Environment"
#define METHAN_OS_UNIX_LIKE
#else
#error "Your platform ain't supported by methan yet"
#endif

/** Detect the Compiler **/
#if defined(__clang__)
#define METHAN_COMPILER_CLANG
#define METHAN_COMPILER_NAME           "Clang"
#elif defined(_MSC_VER)
#define METHAN_COMPILER_MSC
#define METHAN_COMPILER_NAME           "Microsoft Visual C/C++"
#elif defined(__llvm__)
#define METHAN_COMPILER_LLVM
#define METHAN_COMPILER_CLANG
#define METHAN_COMPILER_NAME           "LLVM"
#elif defined(__MINGW32__)
#define METHAN_COMPILER_MINGW
#define METHAN_COMPILER_GCC
#define METHAN_COMPILER_NAME           "Mingw"
#elif defined(__GNUC__)
#define METHAN_COMPILER_GCC
#define METHAN_COMPILER_NAME           "Gnu GCC C/C++"
#endif

/** Detect available hardware acceleration(s) **/
#if defined(METHAN_COMPILER_GCC)
#if defined(__SSE2__)
#define METHAN_SUPPORT_SSE2
#endif
#if defined(__SSE3__)
#define METHAN_SUPPORT_SSE3
#endif
#if defined(__SSSE3__)
#define METHAN_SUPPORT_SSSE3
#endif
#if defined(__SSE4_1__)
#define METHAN_SUPPORT_SSSE41
#endif
#if defined(__SSE4_2__)
#define METHAN_SUPPORT_SSSE42
#endif
#elif defined(METHAN_COMPILER_MSC)
/* On MSVC: https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros */
#if (defined(_M_IX86_FP) && _M_IX86_FP == 2) || defined(_M_AMD64) || defined(_M_X64)
#define METHAN_SUPPORT_SSE2
#endif
#if defined(__AVX__)
#define METHAN_SUPPORT_SSE3
#define METHAN_SUPPORT_SSSE3
#define METHAN_SUPPORT_SSE41
#define METHAN_SUPPORT_SSE42
#endif
#endif

#if defined(METHAN_COMPILER_GCC) || defined(METHAN_COMPILER_MSC)
#if defined(__AVX__)
#define METHAN_SUPPORT_AVX
#endif
#if defined(__AVX2__)
#define METHAN_SUPPORT_AVX2
#endif
#if defined(__AVX512F__)
#define METHAN_SUPPORT_AVX512F
#endif
#endif

#if defined(METHAN_COMPILER_GCC)
#if defined(__F16C__)
#define METHAN_SUPPORT_AVX_F16C
#endif
#if defined(__FMA__)
#define METHAN_SUPPORT_AVX_FMA
#endif
#elif defined(METHAN_COMPILER_MSC) && defined(__AVX2__)
#define METHAN_SUPPORT_AVX_F16C
#define METHAN_SUPPORT_AVX_FMA
#endif


