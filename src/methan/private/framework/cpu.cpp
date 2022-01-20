#include <methan/private/framework/cpu.hpp>
#include <methan/private/private_context.hpp>
#include <methan/core/details/platform.hpp>
#include <methan/private/private_constant.hpp>
#include <methan/private/private_formatter.hpp>
#include <mutex>
#include <chrono>


METHAN_API Methan::Cpu::Cpu(Context context, uint8_t maxCoreUsed)
: AbstractDevice(context, "cpu")
{
    m_descriptor.core = std::thread::hardware_concurrency();
    if(m_descriptor.core > (uint32_t) maxCoreUsed)
        m_descriptor.core = (uint32_t) maxCoreUsed;

    m_descriptor.deviceType = Methan::EDeviceType::Cpu;
    m_descriptor.floops = 0.f;
    m_descriptor.freq = 0.f;
}

METHAN_API Methan::Cpu::~Cpu()
{

}
