#pragma once

#include <methan/core/configuration.hpp>

#ifdef METHAN_BUILD_SHARED
#ifdef METHAN_EXPORT
#define DLL __declspec(dllexport)
#else
#define DLL __declspec(dllimport)
#endif
#else
#define DLL
#endif

DLL int test();
DLL bool returnTrue();