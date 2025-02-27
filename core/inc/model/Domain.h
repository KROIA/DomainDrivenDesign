#pragma once
#include "DDD_base.h"
#include "Repository.h"
#include "Service.h"
#include "AggregateFactory.h"


namespace DDD
{

	template <DerivedFromAggregate AGG>
	class Domain
	{
	public:
		typedef AGG AggregateType;

		Domain()
		{

		}
		Domain(const Domain& other) = delete;
		Domain(const Domain&& other) noexcept
			: m_repository(std::move(other.m_repository))
			, m_services(std::move(other.m_services))
			, m_factories(std::move(other.m_factories))
		{
			
		}
		~Domain() = default;

		
		ID executeFactoryCreateInstance(const ID factoryID, std::shared_ptr<FactoryCreationData> factoryData)
		{
			auto it = m_factories.find(factoryID);
			if (it != m_factories.end())
			{
				return it->second->createAggregateInternal(factoryData);
			}
			return INVALID_ID;
		}
		bool executeFactoryRemoveInstance(ID factoryID, ID aggregateID)
		{
			auto it = m_factories.find(factoryID);
			if (it != m_factories.end())
			{
				return it->second->removeAggregateInternal(aggregateID);
			}
			return false;
		}
		bool executeFactoryReplaceInstance(ID factoryID, std::shared_ptr<FactoryCreationData> factoryData)
		{
			auto it = m_factories.find(factoryID);
			if (it != m_factories.end())
			{
				return it->second->replaceAggregateInternal(factoryData);
			}
			return false;
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

		[[nodiscard]] size_t size() const
		{
			return m_repository.size();
		}
		[[nodiscard]] bool empty() const
		{
			return m_repository.empty();
		}
		[[nodiscard]] bool contains(ID id) const
		{
			return m_repository.contains(id);
		}
		[[nodiscard]] bool contains(const AGG& aggregate) const
		{
			return m_repository.contains(aggregate.id());
		}
		[[nodiscard]] bool contains(const std::shared_ptr<AGG>& aggregate) const
		{
			return m_repository.contains(aggregate->id());
		}
		[[nodiscard]] bool contains(const AGG* aggregate) const
		{
			return m_repository.contains(aggregate->id());
		}

	protected:
		template <typename FAC>
		ID createFactory(const ID factoryID)
		{
			static_assert(std::is_base_of_v<AggregateFactory<AGG>, FAC>, "FAC must derive from AggregateFactory<AGG>");
			std::shared_ptr<AggregateFactory<AGG>> factory = std::make_shared<FAC>(m_repository, factoryID);
			m_factories[factory->getID()] = factory;
			return factory->getID();
		}
		void removeFactory(const ID id)
		{
			m_factories.erase(id);
		}
		template <typename FAC>
		void removeFactory()
		{
			static_assert(std::is_base_of_v<AggregateFactory<AGG>, FAC>, "FAC must derive from AggregateFactory<AGG>");
			for (auto it = m_factories.begin(); it != m_factories.end(); ++it)
			{
				if (dynamic_cast<FAC*>(it->second.get()))
				{
					m_factories.erase(it);
					return;
				}
			}
		}

		template <typename SER>
		ID createService(const ID serviceID)
		{
			static_assert(std::is_base_of_v<Service<AGG>, SER>, "SER must derive from Service<AGG>");
			std::shared_ptr<Service<AGG>> service = std::make_shared<SER>(serviceID);
			m_services[service->getID()] = service;
			return service->getID();
		}
		void removeService(const ID id)
		{
			m_services.erase(id);
		}
		template <typename SER>
		void removeService()
		{
			static_assert(std::is_base_of_v<Service<AGG>, SER>, "SER must derive from Service<AGG>");
			for (auto it = m_services.begin(); it != m_services.end(); ++it)
			{
				if (dynamic_cast<SER*>(it->second.get()))
				{
					m_services.erase(it);
					return;
				}
			}
		}

		/*

		[[nodiscard]] std::shared_ptr<Service<AGG>> getService(ID id)
		{
			auto it = m_services.find(id);
			if (it != m_services.end())
			{
				return it->second;
			}
			return nullptr;
		}*/


		bool addAggregate(const std::shared_ptr<AGG>& aggregate)
		{
			return m_repository.add(aggregate);
		}
		bool removeAggregate(ID id)
		{
			return m_repository.remove(id);
		}
		[[nodiscard]] std::shared_ptr<AGG> getAggregate(ID id)
		{
			return m_repository.get(id);
		}
		[[nodiscard]] std::vector<std::shared_ptr<AGG>> getAllAggregates()
		{
			return m_repository.getAll();
		}


		
		



		
		
		[[nodiscard]] std::shared_ptr<AggregateFactory<AGG>> getFactory(ID id)
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
	};
}