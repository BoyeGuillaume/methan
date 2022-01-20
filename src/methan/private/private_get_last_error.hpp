#pragma once

#include <errno.h>
#include <string>

#include <methan/core/except.hpp>


#define METHAN_LAST_ERROR std::string(strerror(errno))

