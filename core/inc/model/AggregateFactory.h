#pragma once
#include "DDD_base.h"
#include "Repository.h"
#include "utilities/UniqueIDDomain.h"

namespace DDD
{
	template <DerivedFromAggregate AGG>
	class AggregateFactory
	{
	public:
		typedef AGG AggregateType;

		AggregateFactory(Repository<AGG>* repo)
			: m_repository(repo)
		{}
		void unregister()
		{
			m_repository = nullptr;
		}
		const std::string_view& getName() const
		{
			return m_name;
		}
	protected:
		std::shared_ptr<AGG> registerInstance(std::shared_ptr<AGG> agg)
		{
			if (m_repository)
			{
				if (m_repository->add(agg))
					return agg;
			}
#if LOGGER_LIBRARY_AVAILABLE == 1
			Logger::logError("AggregateFactory<AGG>::registerInstance(): Can't register object");
#endif
			return nullptr;
		}
	private:
		Repository<AGG>* m_repository;
		const std::string_view m_name = typeid(AGG).name();
	};
}