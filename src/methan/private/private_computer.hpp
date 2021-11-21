#pragma once

#include <methan/core/computer.hpp>
#include <methan/core/except.hpp>


namespace Methan {

    METHAN_API Computer create_self(Context context, EComputerType computerType);

}
