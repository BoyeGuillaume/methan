#include <methan/core/computer.hpp>
#include <methan/private/private_computer.hpp>
#include <methan/private/private_context.hpp>
#include <methan/private/private_formatter.hpp>
#include <methan/private/private_get_last_error.hpp>

#include <mutex>
#include <cstring>

#if defined(METHAN_OS_WINDOWS)
#include <Windows.h>
#elif defined(METHAN_OS_UNIX_LIKE)
#include <unistd.h>
#include <errno.h>
#endif

METHAN_API Methan::Computer::Computer(Context context, std::string hostname, std::string username, const Uuid& uuid, EComputerType computerType)
: Contextuable(context),
  m_uuid(uuid),
  m_hostname(hostname),
  m_username(username),
  m_computerType(computerType)
{

}

METHAN_API Methan::Computer Methan::create_self(Context context, EComputerType computerType)
{
    Methan::Uuid uuid;
    {
        std::lock_guard<std::mutex> guard(context->__uuid_factory_m);
        uuid = context->uuid_factory();
    }

    
    std::string hostname;
    std::string username;

#if defined(METHAN_OS_WINDOWS)
    {
        char HostnameBuf[METHAN_SMALL_BUFFER_SIZE];
        char UsernameBuf[METHAN_SMALL_BUFFER_SIZE];
        DWORD HostnameBufSize = sizeof(HostnameBuf);
        DWORD UsernameBufSize = sizeof(UsernameBuf);
        
        std::lock_guard<std::recursive_mutex> guard(context->__system_m);
        if(!GetComputerName(HostnameBuf, &HostnameBufSize)) {
            METHAN_THROW_EXCEPTION("The call to `GetComputerName` failed for unknown reason. ErrCode: " + METHAN_LAST_ERROR, ExceptionType::Unknown);
        }

        if(!GetUserName(UsernameBuf, &UsernameBufSize)) {
            METHAN_THROW_EXCEPTION("The call to `GetUserName` failed for unknown reason. ErrCode: " + METHAN_LAST_ERROR, ExceptionType::Unknown);
        }

        hostname = std::string(HostnameBuf);
        username = std::string(UsernameBuf);
    }
#elif defined(METHAN_OS_UNIX_LIKE)
    {
        char HostnameBuf[METHAN_SMALL_BUFFER_SIZE];
        char UsernameBuf[METHAN_SMALL_BUFFER_SIZE];
    
    
        std::lock_guard<std::recursive_mutex> guard(context->__system_m);
        if(!gethostname(HostnameBuf, sizeof(HostnameBuf))) {
            METHAN_THROW_EXCEPTION("The call to `gethostname` failed for unkown reason. ErrCode : " + METHAN_LAST_ERROR, ExceptionType::Unknown);
        }

        if(!getlogin_r(UsernameBuf, sizeof(UsernameBuf))) {
            METHAN_THROW_EXCEPTION("The call to `getlogin_r` failed for unkown reason. ErrCode : " + METHAN_LAST_ERROR, ExceptionType::Unknown);
        }

        hostname = std::string(HostnameBuf);
        username = std::string(UsernameBuf);
    }
#endif

    METHAN_LOG_INFO(context->logger, "Registering self as {}, on [username={}, hostname={}]", uuid, username, hostname);

    return Computer(context, hostname, username, uuid, computerType);
}
