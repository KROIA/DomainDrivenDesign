#pragma once
#include "DDD_base.h"
#include "Repository.h"
#include "utilities/UniqueIDDomain.h"

namespace DDD
{
	/*
	class FactoryCreationData
	{
	public:
		FactoryCreationData() = default;
		FactoryCreationData(const FactoryCreationData& other) = default;
		FactoryCreationData(FactoryCreationData&& other) noexcept = default;
		virtual ~FactoryCreationData() = default;
	};*/

	template <DerivedFromAggregate AGG>
	class AggregateFactory
	{
	public:
		typedef AGG AggregateType;

		AggregateFactory(Repository<AGG>* repo, UniqueIDDomain *idDomain)
			: m_repository(repo)
			, m_idDomain(idDomain)
		{}
		void unregister()
		{
			m_repository = nullptr;
			m_idDomain = nullptr;
		}
	protected:
		ID getNextID()
		{
			if (m_idDomain)
				return m_idDomain->getNextID();
			return INVALID_ID;
		}
		std::shared_ptr<AGG> registerInstance(std::shared_ptr<AGG> agg)
		{
			if (m_repository)
			{
				if (m_repository->add(agg))
					return agg;
			}
			return nullptr;
		}




		Repository<AGG>* m_repository;
		UniqueIDDomain* m_idDomain;

	private:

	};
}