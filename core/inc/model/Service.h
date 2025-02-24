#pragma once
#include "DDD_base.h"
#include "utilities/IID.h"
#include "model/Aggregate.h"


namespace DDD
{
	class ServiceExecutionResult
	{
	public:
		ServiceExecutionResult() {}
		virtual ~ServiceExecutionResult() {}
	};

	template <DerivedFromAggregate T>
	class Service : public IID
	{
		template <DerivedFromAggregate AGG>
		friend class Domain;
	public:
		Service(ID id)
			: IID(id)
		{}

	protected:
		virtual std::shared_ptr<ServiceExecutionResult> execute(const std::unordered_map<ID, std::shared_ptr<T>>& repo) = 0;

	private:
		std::shared_ptr<ServiceExecutionResult> executeInternal(const Repository<T>& repo)
		{
			std::unordered_map<ID, std::shared_ptr<T>> cRepo;
			std::vector<std::shared_ptr<T>> all = repo.getAll();
			for (auto element : all)
			{
				cRepo[element->getID()] = element;
			}
			return execute(cRepo);
		}
	};
}