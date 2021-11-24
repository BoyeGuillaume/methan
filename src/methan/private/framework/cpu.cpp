#include <methan/private/framework/cpu.hpp>
#include <methan/private/private_context.hpp>
#include <methan/core/details/platform.hpp>
#include <methan/private/private_constant.hpp>
#include <methan/private/private_formatter.hpp>
#include <mutex>
#include <chrono>


#ifdef METHAN_OS_WINDOWS
#include <Windows.h>
#endif

METHAN_API Methan::DeviceDescriptor* Methan::cpu_descriptor(Context context)
{
    Methan::DeviceDescriptor* deviceDescriptor = new Methan::DeviceDescriptor{};
    deviceDescriptor->deviceType = Methan::EDeviceType::Cpu;
    deviceDescriptor->uuid = generate_uuid(context);

#ifdef METHAN_OS_WINDOWS
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    deviceDescriptor->core = sysinfo.dwNumberOfProcessors;
#endif
    deviceDescriptor->freq = 0.f;
    deviceDescriptor->floops = 0.f;
    
    METHAN_LOG_INFO(context->logger, "Registing new DeviceDescriptor for the cpu with uuid {}", deviceDescriptor->uuid);

    return deviceDescriptor;
}
