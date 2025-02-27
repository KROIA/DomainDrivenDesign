#pragma once
#include "DDD_base.h"
#include "utilities/IID.h"
#include "model/Aggregate.h"


namespace DDD
{
	class ServiceExecutionResult
	{
	public:
		ServiceExecutionResult() = default;
		ServiceExecutionResult(const ServiceExecutionResult&) = default;
		ServiceExecutionResult(ServiceExecutionResult&&) = default;
		virtual ~ServiceExecutionResult() = default;
	};

	template <DerivedFromAggregate AGG>
	class Service : public IID
	{
		template <DerivedFromAggregate AGG>
		friend class Domain;
	public:
		typedef AGG AggregateType;
		Service(ID id)
			: IID(id)
		{}

	protected:
		virtual std::shared_ptr<ServiceExecutionResult> execute(const std::unordered_map<ID, std::shared_ptr<AGG>>& repo) = 0;

	private:
		std::shared_ptr<ServiceExecutionResult> executeInternal(const Repository<AGG>& repo)
		{
			std::unordered_map<ID, std::shared_ptr<AGG>> cRepo;
			std::vector<std::shared_ptr<AGG>> all = repo.getAll();
			for (const auto &element : all)
			{
				cRepo[element->getID()] = element;
			}
			return execute(cRepo);
		}
	};
}