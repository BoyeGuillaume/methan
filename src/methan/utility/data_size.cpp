#include <methan/utility/data_size.hpp>
#include <sstream>

#define HELPER(x, div)             tstring(((double) (x / (1_ ## div / 10))) / 10.0) + " " #div

inline static std::string tstring(double x)
{
    std::ostringstream out;
    out.precision(1);
    out << std::fixed << x;
    return out.str();
}

METHAN_API std::string Methan::to_string(DataSize size)
{
    if(size >= 1_GB) return HELPER(size, GB);
    else if(size >= 1_MB) return HELPER(size, MB);
    else if(size >= 1_KB) return HELPER(size, KB);
    else return std::to_string(size) + " B";
}
