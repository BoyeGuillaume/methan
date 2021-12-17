#pragma once

#include <methan/core/except.hpp>


#if defined(METHAN_OS_UNIX)
#define METHAN_ATTRIBUTE_NOINLINE                                                 __attribute__((noinline))
#define METHAN_ATTRIBUTE_ALWAYS_INLINE                                            __attribute__((always_inline))
#define METHAN_ATTRIBUTE_UNUSED                                                   __attribute__((unused))
#define METHAN_PACKED                                                             __attribute__((packed))
#elif defined(METHAN_COMPILER_MSC)
#define METHAN_ATTRIBUTE_NOINLINE                                                 
#define METHAN_ATTRIBUTE_ALWAYS_INLINE                                            __forceinline
#define METHAN_ATTRIBUTE_UNUSED                                                   
#define METHAN_PACKED                                                             
#endif

