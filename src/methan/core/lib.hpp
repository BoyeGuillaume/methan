#pragma once

#include <methan/core/configuration.hpp>

#ifdef _WIN64
#define METHAN_PLATFORM_WINDOWS
#define METHAN_ARCH_x64
#define METHAN_WINDOWS_x64
#elif _WIN32
#define METHAN_PLATFORM_WINDOWS
#define METHAN_ARCH_x32
#define METHAN_WINDOWS_x32
#elif __APPLE__
#define METHAN_PLATFORM_APPLE
#elif __linux
#define METHAN_PLATFORM_LINUX
#else 
#error "Unknown or not supported platform"
#endif

#ifdef METHAN_BUILD_SHARED
#ifdef METHAN_PLATFORM_WINDOWS
#ifdef METHAN_EXPORT
#define DLL __declspec(dllexport)
#else
#define DLL __declspec(dllimport)
#endif
#else
#ifdef METHAN_EXPORT
#define DLL __attribute__((visibility("default")))
#else
#define DLL
#endif
#endif
#else
#define DLL
#endif

DLL int test();
DLL bool returnTrue();