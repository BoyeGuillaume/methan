#include <methan/private/framework/operator/operator_registery.hpp>
#include <methan/private/private_context.hpp>
#include <methan/private/private_formatter.hpp>
#include <methan/private/framework/operator/operator_factory.hpp>
#include <functional>
#include <unordered_set>

static std::vector<std::pair<Methan::StringIdentifier, std::function<Methan::AbstractOperatorFactory*(Methan::Context context)>>> __operators;


METHAN_API Methan::OperatorRegistry::OperatorRegistry(Context context)
: Contextuable(context)
{
    METHAN_LOG_DEBUG(context->logger, "OperatorRegistry() construtected");

#ifdef METHAN_EXPAND_ASSERTION
    std::unordered_set<StringIdentifier> identifiers;
#endif

    for(const auto& ops : __operators)
    {
#ifdef METHAN_EXPAND_ASSERTION
        auto it = identifiers.find(ops.first);
        if(it != identifiers.end())
        {
            METHAN_LOG_ERROR(context->logger, "OperatorFactory with name {} already contained in the registry. Did you call METHAN_REGISTER_FACTORY twice ?", std::to_string(ops.first));
            METHAN_INVALID_STATE;
        }
        identifiers.insert(ops.first);
#endif

        AbstractOperatorFactory* factory = ops.second(context);
        m_factories.insert(std::make_pair(ops.first, factory));

        METHAN_LOG_INFO(context->logger, "Found Factory for [id={}]", std::to_string(ops.first));
    }
}

METHAN_API Methan::OperatorRegistry::~OperatorRegistry()
{
    for(const auto& factory : m_factories)
    {
        delete factory.second;
    }

    METHAN_LOG_DEBUG(context()->logger, "OperatorRegistry() deleted");
}

METHAN_API Methan::AbstractOperatorFactory* Methan::OperatorRegistry::find(const StringIdentifier& identifier) const
{
    std::unordered_map<StringIdentifier, AbstractOperatorFactory*>::const_iterator it
        = m_factories.find(identifier);
    if(it == m_factories.end()) return nullptr;
    else return it->second;
}

METHAN_API std::monostate Methan::OperatorRegistry::__register(const StringIdentifier& identifier, const std::function<AbstractOperatorFactory*(Context)>& _constr)
{
    __operators.push_back(std::make_pair(identifier, _constr));
    return std::monostate{};
}
