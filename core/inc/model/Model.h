#pragma once
#include "DDD_base.h"
#include "Repository.h"
#include "Service.h"
#include "AggregateFactory.h"
#include "utilities/UniqueIDDomain.h"
#include "IPersistence.h"
#include <variant>
#include <array>
#include <utility> // for std::move

namespace DDD
{
	class AggregateLock;
	class User;

	template <DerivedFromAggregate... Ts>
	class Model
	{
		template<DerivedFromAggregate AGG>
		class AggregateContainer
		{
		public:
			AggregateContainer(UniqueIDDomain& domain, 
				const std::function<void(const std::vector<ID>&)> &aggregateAddedSignal,
				const std::function<void(const std::vector<ID>&)> &aggregateReplacedSignal,
				const std::function<void(const std::vector<ID>&)> &aggregateRemovedSignal)
				: m_repository(domain)
				, m_aggregateAddedSignal(aggregateAddedSignal)
				, m_aggregateReplacedSignal(aggregateReplacedSignal)
				, m_aggregateRemovedSignal(aggregateRemovedSignal)
			{
			}
#if LOGGER_LIBRARY_AVAILABLE == 1
			void attachLogger(Log::LogObject* logger)
			{
				m_repository.attachLogger(logger);
				Log::LoggerID id = (logger ? logger->getID(): 0);
				if (m_factory)
					m_factory->setLoggerParentID(id);
			}
#endif
			void setFactory(std::shared_ptr<AggregateFactory<AGG>> fac)
			{
				m_factory = fac;
			}
			std::shared_ptr<AggregateFactory<AGG>> getFactory() const
			{
				return m_factory;
			}
			void setServices(const std::vector<std::shared_ptr<AggregateService<AGG>>>& servs)
			{
				m_services = servs;
			}
			void addService(std::shared_ptr<AggregateService<AGG>> serv)
			{
				m_services.push_back(serv);
			}
			template <DerivedFromService SER>
			std::shared_ptr<SER> createService()
			{
				std::shared_ptr<SER> service = std::make_shared<SER>(&m_repository);
				m_services.push_back(service);
				return service;
			}
			void clearServices()
			{
				for (size_t i = 0; i < m_services.size(); ++i) {
					m_services[i]->unregister();
				}
				m_services.clear();
			}
			void removeService(size_t index)
			{
				if (index < m_services.size())
				{
					m_services[index]->unregister();
					m_services.erase(m_services.begin() + index);
				}
			}
			AggregateService<AGG> getService(size_t index) const
			{
				if (index < m_services.size())
				{
					return *m_services[index];
				}
				return nullptr;
			}
			const std::vector<std::shared_ptr<AggregateService<AGG>>>& getServices() const
			{
				return m_services;
			}


			bool isAggregateTypeForThis(std::shared_ptr<Aggregate> agg)
			{
				return dynamic_pointer_cast<AGG>(agg) != nullptr;
			}
			bool add(std::shared_ptr<Aggregate> agg)
			{
				std::shared_ptr<AGG> casted = dynamic_pointer_cast<AGG>(agg);
				if (casted)
				{
					if (m_repository.add(casted))
					{
						if(m_aggregateAddedSignal)
							m_aggregateAddedSignal({ casted->getID() });
						return true;
					}
				}
				return false;
			}
			bool replace(std::shared_ptr<Aggregate> agg)
			{
				std::shared_ptr<AGG> casted = dynamic_pointer_cast<AGG>(agg);
				if (casted)
				{
					if (m_repository.remove(casted->getID()))
					{
						if (m_repository.add(casted))
						{
							if(m_aggregateReplacedSignal)
								m_aggregateReplacedSignal({ casted->getID() });
							return true;
						}
					}
				}
				return false;
			}

			bool remove(ID id)
			{
				if (m_repository.remove(id))
				{
					if(m_aggregateRemovedSignal)
						m_aggregateRemovedSignal({ id });
					return true;
				}
				return false;
			}

			[[nodiscard]] std::shared_ptr<AGG> get(ID id) { return m_repository.get(id); }
			[[nodiscard]] std::shared_ptr<const AGG> get(ID id) const { return m_repository.get(id); }

			[[nodiscard]] std::vector<std::shared_ptr<AGG>> getAll() { return m_repository.getAll(); }
			[[nodiscard]] std::vector<std::shared_ptr<const AGG>> getAll() const { return m_repository.getAll(); }
			[[nodiscard]] std::vector<ID> getIDs() const { return m_repository.getIDs(); }
			void clear() { m_repository.clear(); }

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
				return m_repository.contains(aggregate);
			}
			[[nodiscard]] bool contains(const std::shared_ptr<AGG>& aggregate) const
			{
				return m_repository.contains(aggregate);
			}
			[[nodiscard]] bool contains(const AGG* aggregate) const
			{
				return m_repository.contains(aggregate);
			}

			[[nodiscard]] std::vector<std::shared_ptr<AGG>> getDeleted()
			{
				return m_repository.getDeleted();
			}
			[[nodiscard]] std::vector<std::shared_ptr<const AGG>> getDeleted() const
			{
				return m_repository.getDeleted();
			}
			void clearDeletedCache()
			{
				m_repository.clearDeletedCache();
			}

		private:
			Repository<AGG> m_repository;
			std::shared_ptr<AggregateFactory<AGG>> m_factory;
			std::vector<std::shared_ptr<AggregateService<AGG>>> m_services;
			const std::function<void(const std::vector<ID>&)>& m_aggregateAddedSignal;
			const std::function<void(const std::vector<ID>&)>& m_aggregateReplacedSignal;
			const std::function<void(const std::vector<ID>&)>& m_aggregateRemovedSignal;
		};

	public:
		static constexpr size_t aggregateTypeCount = sizeof...(Ts);

		// Default constructor initializes each instance
		Model()
			: m_idDomain(std::bind(&Model::tryReserveNextID, this, std::placeholders::_1, std::placeholders::_2))
			, m_metadata(std::make_shared<MetadataContainer>())
		{

		}

		void setCallback_aggregateAdded(const std::function<void(const std::vector<ID>&)>& callback)
		{
			m_aggregateAddedSignal = callback;
		}
		void setCallback_aggregateReplaced(const std::function<void(const std::vector<ID>&)>& callback)
		{
			m_aggregateReplacedSignal = callback;
		}
		void setCallback_aggregateRemoved(const std::function<void(const std::vector<ID>&)>& callback)
		{
			m_aggregateRemovedSignal = callback;
		}

		template <typename FAC> std::shared_ptr<FAC> createFactory();
		template <typename FAC> void removeFactory();

		template <DerivedFromService SER> std::shared_ptr<SER> createService();
		template <DerivedFromService SER> void removeService();
		template <DerivedFromAggregate AGG> void removeAllServices();
		void removeAllGeneralServices();

		template <DerivedFromService SER> std::shared_ptr<ServiceExecutionResult> executeService();




		template <DerivedFromAggregate AGG> [[nodiscard]] size_t size() const;
		template <DerivedFromAggregate AGG> [[nodiscard]] bool empty() const;
		template <DerivedFromAggregate AGG> [[nodiscard]] bool contains(ID id) const;
		[[nodiscard]] bool contains(ID id) const;
		template <DerivedFromAggregate AGG> [[nodiscard]] bool contains(const AGG& aggregate) const;
		template <DerivedFromAggregate AGG> [[nodiscard]] bool contains(const std::shared_ptr<typename AGG>& aggregate) const;
		template <DerivedFromAggregate AGG> [[nodiscard]] bool contains(const AGG* aggregate) const;

		template <DerivedFromAggregate AGG> [[nodiscard]] std::shared_ptr<AGG> getAggregate(const ID id);
		template <DerivedFromAggregate AGG> [[nodiscard]] std::shared_ptr<const AGG> getAggregate(const ID id) const;
		[[nodiscard]] std::shared_ptr<Aggregate> getAggregate(const ID id);
		[[nodiscard]] std::shared_ptr<const Aggregate> getAggregate(const ID id) const;
		bool removeAggregate(const ID id);
		template <DerivedFromAggregate AGG> void removeAggregates();
		void clear();
		bool removeDatabase();
		template <DerivedFromAggregate AGG> [[nodiscard]] std::vector<std::shared_ptr<AGG>> getAggregates(const std::vector<ID>& idList);
		template <DerivedFromAggregate AGG> [[nodiscard]] std::vector<std::shared_ptr<const AGG>> getAggregates(const std::vector<ID>& idList) const;
		[[nodiscard]] std::vector<std::shared_ptr<Aggregate>> getAggregates(const std::vector<ID>& idList);
		[[nodiscard]] std::vector<std::shared_ptr<const Aggregate>> getAggregates(const std::vector<ID>& idList) const;
		[[nodiscard]] std::vector<std::shared_ptr<Aggregate>> getAggregates();
		template <DerivedFromAggregate AGG> [[nodiscard]] std::vector<std::shared_ptr<AGG>> getAggregates();
		[[nodiscard]] std::vector<std::shared_ptr<const Aggregate>> getAggregates() const;
		template <DerivedFromAggregate AGG> [[nodiscard]] std::vector<std::shared_ptr<const AGG>> getAggregates() const;

		[[nodiscard]] std::vector<std::shared_ptr<Aggregate>> getDeletedAggregates();
		template <DerivedFromAggregate AGG> [[nodiscard]] std::vector<std::shared_ptr<AGG>> getDeletedAggregates();
		[[nodiscard]] std::vector<std::shared_ptr<const Aggregate>> getDeletedAggregates() const;
		template <DerivedFromAggregate AGG> [[nodiscard]] std::vector<std::shared_ptr<const AGG>> getDeletedAggregates() const;
		void clearDeletedAggregates();
		template <DerivedFromAggregate AGG> void clearDeletedAggregates();

		bool addAggregate(std::shared_ptr<Aggregate> aggregate);
		bool replaceAggregate(std::shared_ptr<Aggregate> aggregate);
		std::vector<bool> addAggregate(std::vector<std::shared_ptr<Aggregate>> aggregates);
		std::vector<bool> replaceAggregate(const std::vector<std::shared_ptr<Aggregate>>& aggregates);

		template <DerivedFromAggregate AGG> [[nodiscard]] std::vector<ID> getIDs() const;
		[[nodiscard]] std::vector<ID> getIDs() const;

		/**
		 * @brief Takes a list of ID's and returns a list of ID's which are from the specific Aggregate type
		 * @details The functions filters the ID's and only returns those which are from the specific Aggregate type
		 */
		template <DerivedFromAggregate AGG> [[nodiscard]] std::vector<ID> filterIDs(const std::vector<ID>& toFilter) const;

		UniqueIDDomain& getIDDomain()
		{
			return m_idDomain;
		}

		template <DerivedFromIPersistance PER> std::shared_ptr<PER>  attachPersistence();
		std::shared_ptr<IPersistence> getPersistance() {
			return m_persistence;
		}
		bool hasPersistanceAttached() const
		{
			return m_persistence != nullptr;
		}
		void removePersistance()
		{
			m_persistence = nullptr;
		}

		bool save() const;
		bool save(const std::vector<ID>& ids) const;
		bool saveMetadata(std::shared_ptr<MetadataContainer::MetaContext> context = nullptr) const;
		bool load();
		bool load(const std::vector<ID>& ids);
		bool loadMetadata(std::shared_ptr<MetadataContainer::MetaContext> context = nullptr);

		bool manualLockDatabase();
		bool manualUnlockDatabase();
		bool isDatabaseManuallyLocked() const;

		bool lockAggregate(const ID& id);
		std::vector<bool> lockAggregate(const std::vector<ID>& id);
		bool unlockAggregate(const ID& id);
		std::vector<bool> unlockAggregate(const std::vector<ID>& id);
		bool tryUnlockAggregateIfLocked(const ID& id);
		bool isAggregateLocked(const ID& id) const;
		std::vector<std::shared_ptr<AggregateLock>> getLockedAggregates() const;
		std::shared_ptr<AggregateLock> getLock(const ID& id) const;

		bool logOnUser(std::shared_ptr<User> user);
		bool logOffUser(std::shared_ptr<User> user);
		std::vector<std::shared_ptr<User>> getLoggedOnUsers() const;


		void setMetadataContainer(std::shared_ptr<MetadataContainer> metadata)
		{
			m_metadata = metadata;
		}
		std::shared_ptr<MetadataContainer> getMetadataContainer() const
		{
			return m_metadata;
		}

#if LOGGER_LIBRARY_AVAILABLE == 1
		void attachLogger(Log::LogObject* logger)
		{
			m_logger = logger;
			if (m_logger)
			{
				m_factoryLogger = new Log::LogObject(m_logger->getID(), "FactoryLogger");
			}
			else
			{
				delete m_factoryLogger;
				m_factoryLogger = nullptr;
			}
			for (auto& agg : m_domains)
			{
				std::visit([this](auto& obj) {
					obj.attachLogger(m_factoryLogger);
					}, agg);
			}
		}
#endif

	protected:
		bool tryReserveNextID(ID id, ID amount);

	private:
		// Retrieve an instance of a specific type X
		template <typename AGG> [[nodiscard]] AggregateContainer<AGG>& getAggregateContainer();
		template <typename AGG> [[nodiscard]] const AggregateContainer<AGG>& getAggregateContainer() const;




		using VariantType = std::variant<AggregateContainer<Ts>...>;
		std::array<VariantType, aggregateTypeCount> m_domains{
			VariantType{AggregateContainer<Ts>(m_idDomain, m_aggregateAddedSignal, m_aggregateReplacedSignal, m_aggregateRemovedSignal)}...
		};

		std::vector<std::shared_ptr<Service>> m_generalServices;
		UniqueIDDomain m_idDomain;
		std::shared_ptr<IPersistence> m_persistence;
		std::shared_ptr<MetadataContainer> m_metadata;


		std::function<void(const std::vector<ID>&)> m_aggregateAddedSignal;
		std::function<void(const std::vector<ID>&)> m_aggregateReplacedSignal;
		std::function<void(const std::vector<ID>&)> m_aggregateRemovedSignal;


#if LOGGER_LIBRARY_AVAILABLE == 1
		Log::LogObject* m_logger = nullptr;
		Log::LogObject* m_factoryLogger = nullptr;
#endif

	};

	template <DerivedFromAggregate... Ts>
	template <typename FAC>
	std::shared_ptr<FAC> Model<Ts...>::createFactory()
	{
		static_assert((std::is_base_of_v<AggregateFactory<typename FAC::AggregateType>, FAC>), "FAC must be derived from AggregateFactory");
		AggregateContainer<typename FAC::AggregateType>& domain = getAggregateContainer<typename FAC::AggregateType>();
		if (domain.getFactory())
		{
#if LOGGER_LIBRARY_AVAILABLE == 1
			if (m_logger) m_logger->debug("Unregistering factory for " + std::string(domain.getFactory()->getAggregateName()));
#endif
			domain.getFactory()->unregister();
		}
		std::shared_ptr<FAC> factory = std::make_shared<FAC>();
#if LOGGER_LIBRARY_AVAILABLE == 1
		if (m_factoryLogger && m_logger)
		{
			//factory->setLoggerParentID(m_factoryLogger->getID());
			m_logger->debug("Registering factory for " + std::string(factory->getAggregateName()));
		}
		factory->setLoggerParentID(m_factoryLogger ? m_factoryLogger->getID() : 0);
#endif
		domain.setFactory(factory);
		return factory;
	}


	template <DerivedFromAggregate... Ts>
	template <typename FAC>
	void Model<Ts...>::removeFactory()
	{
		static_assert((std::is_base_of_v<AggregateFactory<typename FAC::AggregateType>, FAC>), "FAC must be derived from AggregateFactory");
		AggregateContainer<typename FAC::AggregateType>& domain = getAggregateContainer<typename FAC::AggregateType>();
		if (domain.getFactory())
		{
#if LOGGER_LIBRARY_AVAILABLE == 1
			if (m_logger) m_logger->debug("Unregistering factory for " + std::string(domain.getFactory()->getAggregateName()));
#endif
			domain.getFactory()->unregister();
			domain.setFactory(nullptr);
		}
	}



	template <DerivedFromAggregate... Ts>
	template <DerivedFromService SER>
	std::shared_ptr<SER> Model<Ts...>::createService()
	{
		//static_assert((std::is_base_of_v<AggregateService<typename SER::AggregateType>, SER>), "SER must be derived from Service");
		if constexpr (std::is_base_of_v<AggregateService<typename SER::AggregateType>, SER>)
		{
			AggregateContainer<typename SER::AggregateType>& domain = getAggregateContainer<typename SER::AggregateType>();

			// Check if the service already exists
			for (auto& service : domain.getServices()) {
				if (dynamic_cast<SER*>(service.get())) {
#if LOGGER_LIBRARY_AVAILABLE == 1
					if (m_logger) m_logger->error(std::string("Service: ") + typeid(SER).name() + " already exists in the model for the type: " + typeid(Model<Ts...>).name());
#endif
					return nullptr;
				}
			}

			
			std::shared_ptr<SER> service = domain.createService<SER>();
#if LOGGER_LIBRARY_AVAILABLE == 1
			if (m_logger) m_logger->debug("Registering service: " + std::string(service->getName()));
#endif
			return service;
		}
		else
		{
			for (auto& service : m_generalServices)
			{
				if (dynamic_cast<SER*>(service.get())) {
#if LOGGER_LIBRARY_AVAILABLE == 1
					if (m_logger) m_logger->error(std::string("Service: ") + typeid(SER).name() + " already exists in the model for the type: " + typeid(Model<Ts...>).name());
#endif
					return nullptr;
				}
			}
			std::shared_ptr<SER> service = std::make_shared<SER>();
#if LOGGER_LIBRARY_AVAILABLE == 1
			if (m_logger) m_logger->debug("Registering service: " + std::string(service->getName()));
#endif
			m_generalServices.push_back(service);
			return service;
		}
		return nullptr;
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromService SER>
	void Model<Ts...>::removeService()
	{
		//static_assert((std::is_base_of_v<AggregateService<typename SER::AggregateType>, SER>), "SER must be derived from Service");
		if constexpr (std::is_base_of_v<AggregateService<typename SER::AggregateType>, SER>)
		{
			AggregateContainer<typename SER::AggregateType>& domain = getAggregateContainer<typename SER::AggregateType>();

			const auto& services = domain.getServices();
			for (size_t i = 0; i < services.size(); ++i) {
				if (dynamic_cast<SER*>(services[i].get())) {
					domain.removeService(i);
					return;
				}
			}
		}
		else
		{
			for (size_t i = 0; i < m_generalServices.size(); ++i)
			{
				if (dynamic_cast<SER*>(m_generalServices[i].get())) {
					m_generalServices[i]->unregister();
					m_generalServices.erase(m_generalServices.begin() + i);
					return;
				}
			}
		}
#if LOGGER_LIBRARY_AVAILABLE == 1
		if (m_logger) m_logger->debug("Service to remove not found");
#endif
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	void Model<Ts...>::removeAllServices()
	{
		static_assert((std::is_same_v<AGG, Ts> || ...), "Aggregate type <AGG> not found in this model");

		AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		domain.clearServices();

#if LOGGER_LIBRARY_AVAILABLE == 1
		if (m_logger) m_logger->debug("Service to remove not found");
#endif
	}

	template <DerivedFromAggregate... Ts>
	void Model<Ts...>::removeAllGeneralServices()
	{
		/*for (size_t i = 0; i < m_generalServices.size(); ++i) {
			m_generalServices[i]->unregister();
		}*/
		m_generalServices.clear();

#if LOGGER_LIBRARY_AVAILABLE == 1
		if (m_logger) m_logger->debug("Service to remove not found");
#endif
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromService SER>
	std::shared_ptr<ServiceExecutionResult> Model<Ts...>::executeService()
	{
		if constexpr ((std::is_same_v<typename SER::AggregateType, Ts> || ...))
		{
			AggregateContainer<typename SER::AggregateType>& domain = getAggregateContainer<typename SER::AggregateType>();
			const auto& services = domain.getServices();
			for (auto& service : services) {
				if (auto* ptr = dynamic_cast<SER*>(service.get())) {
#if LOGGER_LIBRARY_AVAILABLE == 1
					if (m_logger) m_logger->debug("Executing service: " + std::string(ptr->getName()));
#endif
					return ptr->execute();
				}
			}
		}
		else
		{
			for (auto& service : m_generalServices)
			{
				if (auto* ptr = dynamic_cast<SER*>(service.get())) {
#if LOGGER_LIBRARY_AVAILABLE == 1
					if (m_logger) m_logger->debug("Executing service: " + std::string(ptr->getName()));
#endif
					return ptr->execute();
				}
			}
		}
#if LOGGER_LIBRARY_AVAILABLE == 1
		if (m_logger) m_logger->error("Service: " + std::string(typeid(SER).name()) + " not found for execution.\nService must be created first!");
#endif
		return nullptr;
	}



	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	[[nodiscard]] size_t Model<Ts...>::size() const
	{
		static_assert((std::is_same_v<AGG, Ts> || ...), "Aggregate type <AGG> not found in this model");
		const AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		return domain.size();
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	[[nodiscard]] bool Model<Ts...>::empty() const
	{
		static_assert((std::is_same_v<AGG, Ts> || ...), "Aggregate type <AGG> not found in this model");
		const AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		return domain.empty();
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	[[nodiscard]] bool Model<Ts...>::contains(ID id) const
	{
		static_assert((std::is_same_v<AGG, Ts> || ...), "Aggregate type <AGG> not found in this model");
		const AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		return domain.contains(id);
	}

	template <DerivedFromAggregate... Ts>
	[[nodiscard]] bool Model<Ts...>::contains(ID id) const
	{
		for (auto& agg : m_domains)
		{
			bool found = std::visit([id](auto& obj) {
				return obj.contains(id);
				}, agg);
			if (found)
				return true;
		}
		return false;
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	[[nodiscard]] bool Model<Ts...>::contains(const AGG& aggregate) const
	{
		static_assert((std::is_same_v<AGG, Ts> || ...), "Aggregate type <AGG> not found in this model");
		const AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		return domain.contains(aggregate.getID());
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	[[nodiscard]] bool Model<Ts...>::contains(const std::shared_ptr<typename AGG>& aggregate) const
	{
		static_assert((std::is_same_v<AGG, Ts> || ...), "Aggregate type <AGG> not found in this model");
		const AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		return domain.contains(aggregate->getID());
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	[[nodiscard]] bool Model<Ts...>::contains(const AGG* aggregate) const
	{
		static_assert((std::is_same_v<AGG, Ts> || ...), "Aggregate type <AGG> not found in this model");
		const AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		return domain.contains(aggregate->getID());
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	[[nodiscard]] std::shared_ptr<AGG> Model<Ts...>::getAggregate(const ID id)
	{
		static_assert((std::is_same_v<AGG, Ts> || ...), "Aggregate type <AGG> not found in this model");
		AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		return domain.get(id);
	}
	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	[[nodiscard]] std::shared_ptr<const AGG> Model<Ts...>::getAggregate(const ID id) const
	{
		static_assert((std::is_same_v<AGG, Ts> || ...), "Aggregate type <AGG> not found in this model");
		const AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		return domain.get(id);
	}

	template <DerivedFromAggregate... Ts>
	[[nodiscard]] std::shared_ptr<Aggregate> Model<Ts...>::getAggregate(const ID id)
	{
		std::shared_ptr<Aggregate> objPtr = nullptr;
		for (auto& agg : m_domains)
		{
			std::visit([&objPtr, id](auto& obj) {
				if (!obj.contains(id))
					return;
				objPtr = obj.get(id);
				}, agg);
			if (objPtr)
				return objPtr;
		}
		return nullptr;
	}
	template <DerivedFromAggregate... Ts>
	[[nodiscard]] std::shared_ptr<const Aggregate> Model<Ts...>::getAggregate(const ID id) const
	{
		std::shared_ptr<const Aggregate> objPtr = nullptr;
		for (auto& agg : m_domains)
		{
			std::visit([&objPtr, id](auto& obj) {
				if (!obj.contains(id))
					return;
				objPtr = obj.get(id);
				}, agg);
			if (objPtr)
				return objPtr;
		}
		return nullptr;
	}

	template <DerivedFromAggregate... Ts>
	bool Model<Ts...>::removeAggregate(const ID id)
	{
		for (auto& agg : m_domains)
		{
			bool found = std::visit([id](auto& obj) {
				return obj.remove(id);
				}, agg);
			if (found)
				return true;
		}
		return false;
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	void Model<Ts...>::removeAggregates()
	{
		static_assert((std::is_same_v<AGG, Ts> || ...), "Aggregate type <AGG> not found in this model");
		AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		domain.clear();
	}

	template <DerivedFromAggregate... Ts>
	void Model<Ts...>::clear()
	{
		for (auto& agg : m_domains)
		{
			std::visit([](auto& obj) {
				obj.clear();
				}, agg);
		}
		m_idDomain.reset();
	}

	template <DerivedFromAggregate... Ts>
	bool Model<Ts...>::removeDatabase()
	{
		DDD_GENERAL_PROFILING_FUNCTION(DDD_COLOR_STAGE_1);
		if (hasPersistanceAttached())
			return m_persistence->removeDatabase();
		return false;
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	[[nodiscard]] std::vector<std::shared_ptr<AGG>> Model<Ts...>::getAggregates(const std::vector<ID>& idList)
	{
		static_assert((std::is_same_v<AGG, Ts> || ...), "Aggregate type <AGG> not found in this model");
		std::vector<std::shared_ptr<AGG>> objs;
		AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		objs.reserve(idList.size());
		for (const ID& id : idList)
		{
			if (!domain.contains(id))
				continue;
			std::shared_ptr<AGG> obj = domain.get(id);
			if (obj)
				objs.push_back(obj);
		}
		return objs;
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	[[nodiscard]] std::vector<std::shared_ptr<const AGG>> Model<Ts...>::getAggregates(const std::vector<ID>& idList) const
	{
		static_assert((std::is_same_v<AGG, Ts> || ...), "Aggregate type <AGG> not found in this model");
		std::vector<std::shared_ptr<const AGG>> objs;
		const AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		objs.reserve(idList.size());
		for (const ID& id : idList)
		{
			if (!domain.contains(id))
				continue;
			std::shared_ptr<const AGG> obj = domain.get(id);
			if (obj)
				objs.push_back(obj);
		}
		return objs;
	}

	template <DerivedFromAggregate... Ts>
	[[nodiscard]] std::vector<std::shared_ptr<Aggregate>> Model<Ts...>::getAggregates(const std::vector<ID>& idList)
	{
		std::vector<std::shared_ptr<Aggregate>> objs;
		for (auto& agg : m_domains)
		{
			std::visit([&objs, &idList](auto& obj) {
				for (const ID& id : idList)
				{
					if (!obj.contains(id))
						continue;
					std::shared_ptr<Aggregate> ins = obj.get(id);
					if (ins)
						objs.push_back(ins);
				}
				}, agg);
		}
		return objs;
	}

	template <DerivedFromAggregate... Ts>
	[[nodiscard]] std::vector<std::shared_ptr<const Aggregate>> Model<Ts...>::getAggregates(const std::vector<ID>& idList) const
	{
		std::vector<std::shared_ptr<const Aggregate>> objs;
		for (auto& agg : m_domains)
		{
			std::visit([&objs, &idList](auto& obj) {
				for (const ID& id : idList)
				{
					if (!obj.contains(id))
						continue;
					std::shared_ptr<const Aggregate> ins = obj.get(id);
					if (ins)
						objs.push_back(ins);
				}
				}, agg);
		}
		return objs;
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	[[nodiscard]] std::vector<std::shared_ptr<AGG>> Model<Ts...>::getAggregates()
	{
		AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		return domain.getAll();
	}

	template <DerivedFromAggregate... Ts>
	[[nodiscard]] std::vector<std::shared_ptr<Aggregate>> Model<Ts...>::getAggregates()
	{
		std::vector<std::shared_ptr<Aggregate>> objs;
		for (auto& agg : m_domains)
		{
			std::visit([&objs](auto& obj) {
				for (auto& ins : obj.getAll())
					objs.push_back(ins);
				}, agg);
		}
		return objs;
	}

	template <DerivedFromAggregate... Ts>
	[[nodiscard]] std::vector<std::shared_ptr<const Aggregate>> Model<Ts...>::getAggregates() const
	{
		std::vector<std::shared_ptr<const Aggregate>> objs;
		for (auto& agg : m_domains)
		{
			std::visit([&objs](auto& obj) {
				for (auto& ins : obj.getAll())
					objs.push_back(ins);
				}, agg);
		}
		return objs;
	}
	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	[[nodiscard]] std::vector<std::shared_ptr<const AGG>> Model<Ts...>::getAggregates() const
	{
		const AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		std::vector<std::shared_ptr<const AGG>> objs;
		objs.reserve(domain.size());
		for (const auto& it : domain.getALL())
		{
			objs.push_back(it);
		}
		return objs;
	}

	template <DerivedFromAggregate... Ts>
	[[nodiscard]] std::vector<std::shared_ptr<Aggregate>> Model<Ts...>::getDeletedAggregates()
	{
		std::vector<std::shared_ptr<Aggregate>> objs;
		for (auto& agg : m_domains)
		{
			std::visit([&objs](auto& obj) {
				auto deleted = obj.getDeleted();
				for (auto& ins : deleted)
				{
					objs.push_back(ins);
				}
				}, agg);
		}
		return objs;
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	[[nodiscard]] std::vector<std::shared_ptr<AGG>> Model<Ts...>::getDeletedAggregates()
	{
		AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		return domain.getDeleted();
	}

	template <DerivedFromAggregate... Ts>
	[[nodiscard]] std::vector<std::shared_ptr<const Aggregate>> Model<Ts...>::getDeletedAggregates() const
	{
		std::vector<std::shared_ptr<const Aggregate>> objs;
		for (auto& agg : m_domains)
		{
			std::visit([&objs](auto& obj) {
				auto deleted = obj.getDeleted();
				for (auto& ins : deleted)
				{
					objs.push_back(ins);
				}
				}, agg);
		}
		return objs;
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	[[nodiscard]] std::vector<std::shared_ptr<const AGG>> Model<Ts...>::getDeletedAggregates() const
	{
		AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		return domain.getDeleted();
	}

	template <DerivedFromAggregate... Ts>
	void Model<Ts...>::clearDeletedAggregates()
	{
		for (auto& agg : m_domains)
		{
			std::visit([](auto& obj) {
				obj.clearDeletedCache();
				}, agg);
		}
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	void Model<Ts...>::clearDeletedAggregates()
	{
		AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		domain.clearDeletedCache();
	}



	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	[[nodiscard]] std::vector<ID> Model<Ts...>::getIDs() const
	{
		std::vector<ID> ids;
		const AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		std::vector<ID> subIds = domain.getIDs();
		ids.insert(ids.end(), subIds.begin(), subIds.end());
		return ids;
	}

	template <DerivedFromAggregate... Ts>
	[[nodiscard]] std::vector<ID> Model<Ts...>::getIDs() const
	{
		std::vector<ID> ids;
		for (auto& agg : m_domains)
		{
			std::visit([&ids](auto& obj) {
				std::vector<ID> subIds = obj.getIDs();
				ids.insert(ids.end(), subIds.begin(), subIds.end());
				}, agg);
		}
		return ids;
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	[[nodiscard]] std::vector<ID> Model<Ts...>::filterIDs(const std::vector<ID>& toFilter) const
	{
		std::vector<ID> filteredIDs;
		filteredIDs.reserve(toFilter.size());
		const AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		for (const ID& id : toFilter)
		{
			if (domain.contains(id))
			{
				filteredIDs.push_back(id);
			}
		}
		return filteredIDs;
	}


	template <DerivedFromAggregate... Ts>
	template <DerivedFromIPersistance PER>
	std::shared_ptr<PER> Model<Ts...>::attachPersistence()
	{
		if (m_persistence)
		{
#if LOGGER_LIBRARY_AVAILABLE == 1
			if (m_logger) m_logger->debug("Detaching persistence layer");
#endif
			m_persistence = nullptr;
		}
#if LOGGER_LIBRARY_AVAILABLE == 1
		if (m_logger) m_logger->debug("Attaching persistence layer");
#endif
		std::shared_ptr<PER> persistence = std::make_shared<PER>();
		m_persistence = persistence;
		return persistence;
	}

	template <DerivedFromAggregate... Ts>
	bool Model<Ts...>::addAggregate(std::shared_ptr<Aggregate> aggregate)
	{
		ID id = aggregate->getID();
		if (id == INVALID_ID)
		{
			m_idDomain.setUniqueIDFor(aggregate);
		}
		else if(contains(id))
			return false;


		bool result = false;
		for (auto& agg : m_domains)
		{
			std::visit([&aggregate, &result](auto& obj) {
				if (obj.isAggregateTypeForThis(aggregate))
					result = obj.add(aggregate);
				}, agg);
		}
		return result;
	}
	template <DerivedFromAggregate... Ts>
	bool Model<Ts...>::replaceAggregate(std::shared_ptr<Aggregate> aggregate)
	{
		bool result = false;
		for (auto& agg : m_domains)
		{
			std::visit([&aggregate, &result](auto& obj) {
				if (obj.isAggregateTypeForThis(aggregate))
					result = obj.replace(aggregate);
				}, agg);
		}
		return result;
	}


	template <DerivedFromAggregate... Ts>
	std::vector<bool> Model<Ts...>::addAggregate(std::vector<std::shared_ptr<Aggregate>> aggregates)
	{
		// Check if object with same ID already exists
		size_t newIDsCount = 0;
		for (auto& aggregate : aggregates)
		{
			ID id = aggregate->getID();
			if (id == INVALID_ID)
			{
				++newIDsCount;
			}
			else if (contains(id))
			{
				aggregate = nullptr;
			}
		}
		if(newIDsCount > 0)
		{
			std::vector<std::shared_ptr<IID>> newIDAggregates;
			newIDAggregates.reserve(newIDsCount);
			for (auto& aggregate : aggregates)
			{
				if (!aggregate)
					continue;
				ID id = aggregate->getID();
				if (id == INVALID_ID)
				{
					newIDAggregates.push_back(aggregate);
				}
			}
			m_idDomain.setUniqueIDFor(newIDAggregates);
		}

		std::vector<bool> results(aggregates.size(), false);
		for (auto& agg : m_domains)
		{
			std::visit([&aggregates, &results](auto& obj) {
				for (size_t i = 0; i < aggregates.size(); ++i)
				{
					auto aggregate = aggregates[i];
					if(aggregate == nullptr)
						continue;
					if (obj.isAggregateTypeForThis(aggregate))
						results[i] = obj.add(aggregate);
				}
				}, agg);
		}
		return results;
	}

	template <DerivedFromAggregate... Ts>
	std::vector<bool> Model<Ts...>::replaceAggregate(const std::vector<std::shared_ptr<Aggregate>>& aggregates)
	{
		std::vector<bool> results(aggregates.size(), false);
		for (auto& agg : m_domains)
		{
			std::visit([&aggregates, &results](auto& obj) {
				for (size_t i = 0; i < aggregates.size(); ++i)
				{
					auto aggregate = aggregates[i];
					if (obj.isAggregateTypeForThis(aggregate))
						results[i] = obj.replace(aggregate);
				}
				}, agg);
		}
		return results;
	}


	template <DerivedFromAggregate... Ts>
	bool Model<Ts...>::save() const
	{
		if (!m_persistence)
		{
#if LOGGER_LIBRARY_AVAILABLE == 1
			if (m_logger) m_logger->error("No persistence layer attached to the model");
#endif
			return false;
		}
		else
		{
			return m_persistence->save();
		}
	}
	template <DerivedFromAggregate... Ts>
	bool Model<Ts...>::save(const std::vector<ID>& ids) const
	{
		if (!m_persistence)
		{
#if LOGGER_LIBRARY_AVAILABLE == 1
			if (m_logger) m_logger->error("No persistence layer attached to the model");
#endif
			return false;
		}
		else
		{
			return m_persistence->save(ids);
		}
	}

	template <DerivedFromAggregate... Ts>
	bool Model<Ts...>::saveMetadata(std::shared_ptr<MetadataContainer::MetaContext> context) const
	{
		if (!m_metadata)
			return false;
		m_metadata->onSaveBegin(context);
		if (m_persistence->save(m_metadata))
		{
			m_metadata->onSaveEnd(context);
			return true;
		}
		return false;
	}

	template <DerivedFromAggregate... Ts>
	bool Model<Ts...>::load()
	{
		if (!m_persistence)
		{
#if LOGGER_LIBRARY_AVAILABLE == 1
			if (m_logger) m_logger->error("No persistence layer attached to the model");
#endif
			return false;
		}
		else
		{
			return m_persistence->load();
		}
	}
	template <DerivedFromAggregate... Ts>
	bool Model<Ts...>::load(const std::vector<ID>& ids)
	{
		if (!m_persistence)
		{
#if LOGGER_LIBRARY_AVAILABLE == 1
			if (m_logger) m_logger->error("No persistence layer attached to the model");
#endif
			return false;
		}
		else
		{
			return m_persistence->load(ids);
		}
	}

	template <DerivedFromAggregate... Ts>
	bool Model<Ts...>::loadMetadata(std::shared_ptr<MetadataContainer::MetaContext> context)
	{
		if (!m_metadata)
			return false;
		m_metadata->onLoadBegin(context);
		if (m_persistence->load(m_metadata))
		{
			m_metadata->onLoadEnd(context);
			return true;
		}
		return false;
	}

	template <DerivedFromAggregate... Ts>
	bool Model<Ts...>::manualLockDatabase()
	{
		if (!m_persistence)
			return false;
#if LOGGER_LIBRARY_AVAILABLE == 1
		bool wasLocked = m_persistence->isDatabaseLocked();
#endif
		if (m_persistence->lockDatabase())
		{
#if LOGGER_LIBRARY_AVAILABLE == 1
			if (m_logger && wasLocked != m_persistence->isDatabaseLocked())
				m_logger->debug("Locked database manually");
#endif
			return true;
		}
		return false;
	}

	template <DerivedFromAggregate... Ts>
	bool Model<Ts...>::manualUnlockDatabase()
	{
		if (!m_persistence)
			return false;
		if (m_persistence->unlockDatabase())
		{
#if LOGGER_LIBRARY_AVAILABLE == 1
			if (m_logger && !m_persistence->isDatabaseLocked())
				m_logger->debug("Unlocked database manually");
#endif
			return true;
		}
		return false;
	}

	template <DerivedFromAggregate... Ts>
	bool Model<Ts...>::isDatabaseManuallyLocked() const
	{
		if (!m_persistence)
			return false;
		return m_persistence->isDatabaseLocked();
	}

	template <DerivedFromAggregate... Ts>
	bool Model<Ts...>::lockAggregate(const ID& id)
	{
		if (!m_persistence)
		{
#if LOGGER_LIBRARY_AVAILABLE == 1
			if (m_logger) m_logger->error("No persistence layer attached to the model");
#endif
			return false;
		}
		else
		{
			return m_persistence->lock(id);
		}
	}


	template <DerivedFromAggregate... Ts>
	std::vector<bool> Model<Ts...>::lockAggregate(const std::vector<ID>& ids)
	{
		if (!m_persistence)
		{
#if LOGGER_LIBRARY_AVAILABLE == 1
			if (m_logger) 
				m_logger->error("No persistence layer attached to the model");
#endif
			return std::vector<bool>(ids.size(), false);
		}
		else
		{
			return m_persistence->lock(ids);
		}
	}

	template <DerivedFromAggregate... Ts>
	bool Model<Ts...>::unlockAggregate(const ID& id)
	{
		if (!m_persistence)
		{
#if LOGGER_LIBRARY_AVAILABLE == 1
			if (m_logger) 
				m_logger->error("No persistence layer attached to the model");
#endif
			return false;
		}
		else
		{
			return m_persistence->unlock(id);
		}
	}

	template <DerivedFromAggregate... Ts>
	std::vector<bool> Model<Ts...>::unlockAggregate(const std::vector<ID>& ids)
	{
		if (!m_persistence)
		{
#if LOGGER_LIBRARY_AVAILABLE == 1
			if (m_logger) 
				m_logger->error("No persistence layer attached to the model");
#endif
			return std::vector<bool>(ids.size(), false);
		}
		else
		{
			return m_persistence->unlock(ids);
		}
	}

	template <DerivedFromAggregate... Ts>
	bool Model<Ts...>::tryUnlockAggregateIfLocked(const ID& id)
	{
		if (!m_persistence)
		{
#if LOGGER_LIBRARY_AVAILABLE == 1
			if (m_logger) 
				m_logger->error("No persistence layer attached to the model");
#endif
			return false;
		}
		else
		{
			return m_persistence->tryUnlockIfLocked(id);
		}
	}

	template <DerivedFromAggregate... Ts>
	bool Model<Ts...>::isAggregateLocked(const ID& id) const
	{
		if (!m_persistence)
		{
#if LOGGER_LIBRARY_AVAILABLE == 1
			if (m_logger) m_logger->error("No persistence layer attached to the model");
#endif
			return false;
		}
		else
		{
			return m_persistence->isLocked(id);
		}
	}

	template <DerivedFromAggregate... Ts>
	std::vector<std::shared_ptr<AggregateLock>> Model<Ts...>::getLockedAggregates() const
	{
		if (!m_persistence)
		{
#if LOGGER_LIBRARY_AVAILABLE == 1
			if (m_logger) m_logger->error("No persistence layer attached to the model");
#endif
			return {};
		}
		else
		{
			return m_persistence->getLocks();
		}
	}

	template <DerivedFromAggregate... Ts>
	std::shared_ptr<AggregateLock> Model<Ts...>::getLock(const ID& id) const
	{
		if (!m_persistence)
		{
#if LOGGER_LIBRARY_AVAILABLE == 1
			if (m_logger) m_logger->error("No persistence layer attached to the model");
#endif
			return nullptr;
		}
		else
		{
			return m_persistence->getLock(id);
		}
	}

	template <DerivedFromAggregate... Ts>
	bool Model<Ts...>::logOnUser(std::shared_ptr<User> user)
	{
		if (!m_persistence)
		{
#if LOGGER_LIBRARY_AVAILABLE == 1
			if (m_logger) m_logger->error("No persistence layer attached to the model");
#endif
			return false;
		}
		else
		{
			return m_persistence->logOnUser(user);
		}
	}

	template <DerivedFromAggregate... Ts>
	bool Model<Ts...>::logOffUser(std::shared_ptr<User> user)
	{
		if (!m_persistence)
		{
#if LOGGER_LIBRARY_AVAILABLE == 1
			if (m_logger) m_logger->error("No persistence layer attached to the model");
#endif
			return false;
		}
		else
		{
			return m_persistence->logOffUser(user);
		}
	}

	template <DerivedFromAggregate... Ts>
	std::vector<std::shared_ptr<User>> Model<Ts...>::getLoggedOnUsers() const
	{
		if (!m_persistence)
		{
#if LOGGER_LIBRARY_AVAILABLE == 1
			if (m_logger) m_logger->error("No persistence layer attached to the model");
#endif
			return {};
		}
		else
		{
			return m_persistence->getLoggedOnUsers();
		}
	}

	//
	// PRIVATE
	//

	template <DerivedFromAggregate... Ts>
	bool Model<Ts...>::tryReserveNextID(ID id, ID amount)
	{
		if (!m_metadata || !m_persistence)
			return true;
		if (manualLockDatabase())
		{
			bool success = false;
			std::shared_ptr<MetadataContainer::NewAggregateIDContext> context = std::make_shared<MetadataContainer::NewAggregateIDContext>();
			if(loadMetadata(context))
		//	if (m_persistence->load(m_metadata))
			{
				ID highestID = m_metadata->getCurrentHighestID();
				if (id > highestID && (id + amount) > highestID)
				{
					m_metadata->setCurrentHighestID(id + amount);
					success = saveMetadata(context);
					//success = m_persistence->save(m_metadata);
				}
			}
			else
			{
				// Try to save new metadata anyway
				m_metadata->setCurrentHighestID(id + amount);
				success = saveMetadata(context);
				//success = m_persistence->save(m_metadata);
			}
			success &= manualUnlockDatabase();
			return success;
		}
		return false;
	}

	template <DerivedFromAggregate... Ts>
	template <typename AGG>
	[[nodiscard]] Model<Ts...>::AggregateContainer<AGG>& Model<Ts...>::getAggregateContainer() {
		// Check if D exists in the variant array, otherwise static assert
		static_assert((std::is_same_v<AGG, Ts> || ...), "Aggregate type <AGG> not found in this model");

		for (auto& var : m_domains) {
			if (auto* ptr = std::get_if<AggregateContainer<AGG>>(&var)) {
				return *ptr;
			}
		}
		throw std::runtime_error("Aggregate container not found");
	}

	template <DerivedFromAggregate... Ts>
	template <typename AGG>
	[[nodiscard]] const  Model<Ts...>::AggregateContainer<AGG>& Model<Ts...>::getAggregateContainer() const {
		// Check if D exists in the variant array, otherwise static assert
		static_assert((std::is_same_v<AGG, Ts> || ...), "Aggregate type <AGG> not found in this model");

		for (auto& var : m_domains) {
			if (auto* ptr = std::get_if<AggregateContainer<AGG>>(&var)) {
				return *ptr;
			}
		}
		throw std::runtime_error("Aggregate container not found");
	}
}

// lvalue + lvalue
template <typename T>
std::vector<T> operator+(const std::vector<T>& a, const std::vector<T>& b) {
	std::vector<T> result;
	result.reserve(a.size() + b.size());
	result.insert(result.end(), a.begin(), a.end());
	result.insert(result.end(), b.begin(), b.end());
	return result;
}

// rvalue + lvalue
template <typename T>
std::vector<T> operator+(std::vector<T>&& a, const std::vector<T>& b) {
	a.reserve(a.size() + b.size());
	a.insert(a.end(), b.begin(), b.end());
	return std::move(a);
}

// lvalue + rvalue
template <typename T>
std::vector<T> operator+(const std::vector<T>& a, std::vector<T>&& b) {
	b.insert(b.begin(), a.begin(), a.end());
	return std::move(b);
}

// rvalue + rvalue
template <typename T>
std::vector<T> operator+(std::vector<T>&& a, std::vector<T>&& b) {
	a.reserve(a.size() + b.size());
	a.insert(a.end(), std::make_move_iterator(b.begin()), std::make_move_iterator(b.end()));
	return std::move(a);
}