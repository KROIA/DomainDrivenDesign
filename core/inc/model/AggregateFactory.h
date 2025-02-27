#pragma once
#include "DDD_base.h"
#include "Repository.h"
#include "utilities/IID.h"

namespace DDD
{
	class FactoryCreationData
	{
	public:
		FactoryCreationData() = default;
		FactoryCreationData(const FactoryCreationData& other) = default;
		FactoryCreationData(FactoryCreationData&& other) noexcept = default;
		virtual ~FactoryCreationData() = default;
	};

	template <DerivedFromAggregate AGG>
	class AggregateFactory : public IID
	{
		template <DerivedFromAggregate AGG>
		friend class Domain;
	public:
		typedef AGG AggregateType;

		AggregateFactory(Repository<AGG>& repo, const ID id)
			: IID(id)
			, m_repository(repo)
		{}
		


	protected:
		

		virtual std::shared_ptr<AGG> createAggregate(std::shared_ptr<FactoryCreationData> data) = 0;
		virtual bool removeAggregate(const ID id)
		{
			return m_repository.remove(id);
		}


		Repository<AGG>& m_repository;

	private:
		
		ID createAggregateInternal(std::shared_ptr<FactoryCreationData> data)
		{
			std::shared_ptr<AGG> aggregate = createAggregate(data);
			if(m_repository.add(aggregate))
				return aggregate->getID();
	
			return INVALID_ID;
		}
		ID replaceAggregateInternal(std::shared_ptr<FactoryCreationData> data)
		{
			std::shared_ptr<AGG> aggregate = createAggregate(data);
			if(m_repository.replace(aggregate))
				return aggregate->getID();
			return INVALID_ID;
		}
		bool removeAggregateInternal(const ID id)
		{
			return removeAggregate(id);
		}		
	};
}