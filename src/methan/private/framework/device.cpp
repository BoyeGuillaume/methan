#include <algorithm>
#include <methan/private/framework/device.hpp>
#include <methan/private/private_context.hpp>
#include <methan/private/private_formatter.hpp>

METHAN_API Methan::AbstractDevice::AbstractDevice(Context context, std::string name)
: Contextuable(context), 
  m_name(std::move(name))
{
  m_descriptor.uuid = generate_uuid(context);
  METHAN_LOG_INFO(context->logger, "Device::Device(\"{}\", {}) registered", m_name, m_descriptor.uuid);
  context->devices.push_back(this);
}

METHAN_API Methan::AbstractDevice::~AbstractDevice()
{
    METHAN_LOG_INFO(context()->logger, "Device::Device(\"{}\", {}) destroyed", name(), m_descriptor.uuid);
    auto rIterator = std::remove(context()->devices.begin(), context()->devices.end(), this);
    context()->devices.erase(rIterator, context()->devices.end());
}
