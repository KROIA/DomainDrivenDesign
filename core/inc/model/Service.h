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

	class Service
	{
	public:
		typedef void AggregateType;
		Service(const std::string_view &name)
			: m_name(name)
		{
			
		}
		Service(const Service&) = default;
		Service(Service&&) = default;
		virtual ~Service() = default;

		virtual std::shared_ptr<ServiceExecutionResult> execute() = 0;
		const std::string_view& getName() const
		{
			return m_name;
		}

	protected:
		const std::string_view m_name;
	private:

	};

	template <DerivedFromAggregate AGG>
	class AggregateService : public Service
	{
	public:
		typedef AGG AggregateType;
		AggregateService(Repository<AGG>* repository)
			: Service(typeid(AGG).name())
			, m_repository(repository)
		{}

		void unregister()
		{
			m_repository = nullptr;
		}
		Repository<AGG>* getRepository() const
		{
			return m_repository;
		}

		std::vector<std::shared_ptr<AGG>> getAggregates() const 
		{
			if (m_repository)
				return m_repository->getAll();
			return {};
		}

		virtual std::shared_ptr<ServiceExecutionResult> execute() = 0;


	protected:
		Repository<AGG>* m_repository = nullptr;
	private:
		

	};
}