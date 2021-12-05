#include <methan/private/framework/operator/operator_factory.hpp>

METHAN_API Methan::AbstractOperatorFactory::AbstractOperatorFactory(Context context, const StringIdentifier& identifier)
: Contextuable(context),
  m_identifier(identifier)
{

}

METHAN_API Methan::AbstractOperatorFactory::~AbstractOperatorFactory()
{

}

