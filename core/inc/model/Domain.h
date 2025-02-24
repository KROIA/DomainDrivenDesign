#pragma once
#include "DDD_base.h"
#include "Aggregate.h"
#include "Repository.h"
#include "Service.h"
#include "AggregateFactory.h"

namespace DDD
{

	template <DerivedFromAggregate AGG>
	class Domain
	{
	public:
		Domain()
			: m_factoryIDCounter(INVALID_ID)
		{

		}
		~Domain()
		{

		}

		template <typename FAC>
		ID createFactory()
		{
			static_assert(std::is_base_of<AggregateFactory<AGG>, FAC>::value, "FAC must derive from AggregateFactory<AGG>");
			std::shared_ptr<AggregateFactory<AGG>> factory = std::make_shared<FAC>(m_repository, ++m_factoryIDCounter);
			m_factories[factory->getID()] = factory;
			return factory->getID();
		}
		void removeFactory(ID id)
		{
			m_factories.erase(id);
		}
		ID executeFactoryCreateInstance(ID factoryID, std::shared_ptr<FactoryCreationData> factoryData)
		{
			auto it = m_factories.find(factoryID);
			if (it != m_factories.end())
			{
				return it->second->createAggregateInternal(factoryData);
			}
			return INVALID_ID;
		}
		void executeFactoryRemoveInstance(ID factoryID, ID aggregateID)
		{
			auto it = m_factories.find(factoryID);
			if (it != m_factories.end())
			{
				it->second->removeAggregateInternal(aggregateID);
			}
		}


		std::shared_ptr<ServiceExecutionResult> executeService(ID id)
		{
			auto it = m_services.find(id);
			if (it != m_services.end())
			{
				return it->second->executeInternal(m_repository);
			}
			return nullptr;
		}

	protected:
		void addAggregate(const std::shared_ptr<AGG>& aggregate)
		{
			m_repository.add(aggregate);
		}
		void removeAggregate(ID id)
		{
			m_repository.remove(id);
		}
		std::shared_ptr<AGG> getAggregate(ID id)
		{
			return m_repository.get(id);
		}
		std::vector<std::shared_ptr<AGG>> getAllAggregates()
		{
			return m_repository.getAll();
		}


		void addService(const std::shared_ptr<Service<AGG>>& service)
		{
			m_services[service->getID()] = service;
		}
		void removeService(ID id)
		{
			m_services.erase(id);
		}
		std::shared_ptr<Service<AGG>> getService(ID id)
		{
			auto it = m_services.find(id);
			if (it != m_services.end())
			{
				return it->second;
			}
			return nullptr;
		}
		



		
		
		std::shared_ptr<AggregateFactory<AGG>> getFactory(ID id)
		{
			auto it = m_factories.find(id);
			if (it != m_factories.end())
			{
				return it->second;
			}
			return nullptr;
		}
		


	private:
		Repository<AGG> m_repository;
		std::unordered_map<ID, std::shared_ptr<Service<AGG>>> m_services;
		std::unordered_map<ID, std::shared_ptr<AggregateFactory<AGG>>> m_factories;
		ID m_factoryIDCounter;

	};
}