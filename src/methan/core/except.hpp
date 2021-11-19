#pragma once

#include <stdexcept>

#include <methan/core/details/platform.hpp>
#include <methan/core/details/macro.hpp>
#include <methan/core/configuration.hpp>

#define METHAN_EXPAND(x)                                             __METHAN_EXPAND_DETAILS(x)
#define METHAN_CONCATENATE(a, b)                                     __METHAN_CONCATENATE_DETAILS(a, b)
#define METHAN_STRINGIZE(x)                                          __METHAN_STRINGIZE_DETAILS(x)
#define METHAN_ECAPSULATE_LINE_DETAILS(line)                         __METHAN_ECAPSULATE_LINE_DETAILS(line)
#define METHAN_PP_NARGS(...)                                         __METHAN_PP_NARG_DETAILS(__VA_ARGS__, __METHAN_PP_RSEQ_N_DETAILS())


/**
 * @brief Create an opaque handle structure with the given name. We will define
 * a pointer to that handle. The handle in itself will have then name __private__::__handle
 */
#define METHAN_OPAQUE_HANDLE(handle)                                 \
    namespace __private__ {                                          \
        struct METHAN_CONCATENATE(__, handle);                       \
    }                                                                \
    typedef METHAN_CONCATENATE(__private__::__, handle) *handle

#define METHAN_DISABLE_COPY(className)                               \
    className(const className&) = delete;                            \
    className& operator=(const className&) = delete

#define METHAN_DISABLE_MOVE(className)                               \
    className(className&&) = delete;                                 \
    className& operator=(className&&) = delete

#define METHAN_DISABLE_COPY_MOVE(className)                          \
    METHAN_DISABLE_COPY(className);                                  \
    METHAN_DISABLE_MOVE(className)

/**
 * @brief Tools to execute codes only in debug or release mode
 */
#ifdef METHAN_DEBUG
#define METHAN_DEBUG_ONLY(...)                                       METHAN_EXPAND(__VA_ARGS__)
#define METHAN_RELEASE_ONLY(...)
#define METHAN_DEBUG_OR_RELEASE(debug, release)                      debug
#else
#define METHAN_RELEASE_ONLY(...)                                     METHAN_EXPAND(__VA_ARGS__)
#define METHAN_DEBUG_ONLY(...)
#define METHAN_DEBUG_OR_RELEASE(debug, release)                      release
#endif

/**
 * @brief Define the export policy of the Methan project (shared / static)
 */
#ifdef METHAN_BUILD_SHARED
#if defined(METHAN_COMPILER_MSC)
#ifdef METHAN_EXPORT
#define METHAN_API                                                   __declspec(dllexport)
#else
#define METHAN_API                                                   __declspec(dllimport)
#endif
#elif defined(METHAN_COMPILER_GCC) || defined(METHAN_COMPILER_CLANG)
#ifdef METHAN_EXPORT
#define METHAN_API                                                   __attribute__((visibility("default")))
#else
#define METHAN_API
#endif
#endif
#else
#define METHAN_API
#endif
