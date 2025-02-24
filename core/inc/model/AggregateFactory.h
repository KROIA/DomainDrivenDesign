#pragma once
#include "DDD_base.h"
#include "Aggregate.h"
#include "Repository.h"
#include "utilities/IID.h"


namespace DDD
{
	class FactoryCreationData
	{
	public:
		FactoryCreationData() {}
		virtual ~FactoryCreationData() {}
	};

	template <DerivedFromAggregate T>
	class AggregateFactory : public IID
	{
		template <DerivedFromAggregate AGG>
		friend class Domain;
	public:
		
		virtual ~AggregateFactory() {}
		AggregateFactory(Repository<T>& repo, ID id)
			: IID(id)
			, m_repository(repo)
		{}
		


	protected:
		

		virtual std::shared_ptr<T> createAggregate(std::shared_ptr<FactoryCreationData> data) = 0;
		virtual void removeAggregate(ID id)
		{
			m_repository.remove(id);
		}


		Repository<T>& m_repository;

	private:
		
		ID createAggregateInternal(std::shared_ptr<FactoryCreationData> data)
		{
			std::shared_ptr<T> aggregate = createAggregate(data);
			m_repository.add(aggregate);
			return aggregate->getID();
		}
		void removeAggregateInternal(ID id)
		{
			removeAggregate(id);
		}

		
	};
}