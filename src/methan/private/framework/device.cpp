#include <algorithm>
#include <methan/private/framework/device.hpp>
#include <methan/private/private_context.hpp>
#include <methan/private/private_formatter.hpp>

METHAN_API Methan::Device::Device(Context context, DeviceDescriptor descriptor, std::string name)
: Contextuable(context), 
  m_descriptor(descriptor),
  m_name(std::move(name))
{
    METHAN_LOG_INFO(context->logger, "Device::Device(\"{}\", {}) registered", name, descriptor.uuid);
    context->devices.push_back(this);
}

METHAN_API Methan::Device::~Device()
{
    METHAN_LOG_INFO(context()->logger, "Device::Device(\"{}\", {}) destroyed", name(), m_descriptor.uuid);
    auto rIterator = std::remove(context()->devices.begin(), context()->devices.end(), this);
    context()->devices.erase(rIterator, context()->devices.end());
}
