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
	template <DerivedFromAggregate... Ts>
	class Model
	{
		template<DerivedFromAggregate AGG>
		struct AggregateContainer
		{
			Repository<AGG> repository;
			std::shared_ptr<AggregateFactory<AGG>> factory;
			std::vector<std::shared_ptr<AggregateService<AGG>>> services;

			AggregateContainer(UniqueIDDomain& domain)
				: repository(domain)
			{}
		};

	public:
		static constexpr size_t aggregateTypeCount = sizeof...(Ts);

		// Default constructor initializes each instance
		Model() {

		}

		template <typename FAC> std::shared_ptr<FAC> createFactory();

		template <DerivedFromService SER> std::shared_ptr<SER> createService();
		template <DerivedFromService SER> std::shared_ptr<ServiceExecutionResult> executeService();
		



		template <DerivedFromAggregate AGG> [[nodiscard]] size_t size() const;
		template <DerivedFromAggregate AGG> [[nodiscard]] bool empty() const;
		template <DerivedFromAggregate AGG> [[nodiscard]] bool contains(ID id) const;
		[[nodiscard]] bool contains(ID id) const;
		template <DerivedFromAggregate AGG> [[nodiscard]] bool contains(const AGG& aggregate) const;
		template <DerivedFromAggregate AGG> [[nodiscard]] bool contains(const std::shared_ptr<typename AGG>& aggregate) const;
		template <DerivedFromAggregate AGG>	[[nodiscard]] bool contains(const AGG* aggregate) const;

		template <DerivedFromAggregate AGG> [[nodiscard]] std::shared_ptr<AGG> getAggregate(const ID id);
		template <DerivedFromAggregate AGG> [[nodiscard]] std::shared_ptr<const AGG> getAggregate(const ID id) const;
		[[nodiscard]] std::shared_ptr<Aggregate> getAggregate(const ID id);
		[[nodiscard]] std::shared_ptr<const Aggregate> getAggregate(const ID id) const;
		bool removeAggregate(const ID id);
		template <DerivedFromAggregate AGG> void removeAggregates();
		void clear();
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

		bool save() const;
		bool save(const std::vector<ID>& ids) const;
		bool load();
		bool load(const std::vector<ID>& ids);


		

#if LOGGER_LIBRARY_AVAILABLE == 1
		void attachLogger(Log::LogObject* logger)
		{
			m_logger = logger;
			for (auto& agg : m_domains)
			{
				std::visit([logger](auto& obj) {
					obj.repository.attachLogger(logger);
					}, agg);
			}
		}
#endif

	private:
		// Retrieve an instance of a specific type X
		template <typename AGG> [[nodiscard]] AggregateContainer<AGG>& getAggregateContainer();
		template <typename AGG> [[nodiscard]] const AggregateContainer<AGG>& getAggregateContainer() const;

		

		
		using VariantType = std::variant<AggregateContainer<Ts>...>;
		std::array<VariantType, aggregateTypeCount> m_domains{
			VariantType{AggregateContainer<Ts>(m_idDomain)}...
		};

		std::vector<std::shared_ptr<Service>> m_generalServices;
		UniqueIDDomain m_idDomain;
		std::shared_ptr<IPersistence> m_persistence;

#if LOGGER_LIBRARY_AVAILABLE == 1
		Log::LogObject* m_logger = nullptr;
#endif

	};

	template <DerivedFromAggregate... Ts>
	template <typename FAC>
	std::shared_ptr<FAC> Model<Ts...>::createFactory()
	{
		static_assert((std::is_base_of_v<AggregateFactory<typename FAC::AggregateType>, FAC>), "FAC must be derived from AggregateFactory");
		AggregateContainer<typename FAC::AggregateType>& domain = getAggregateContainer<typename FAC::AggregateType>();
		if (domain.factory)
		{
#if LOGGER_LIBRARY_AVAILABLE == 1
			if (m_logger) m_logger->info("Unregistering factory for "+ std::string(domain.factory->getName()));
#endif
			domain.factory->unregister();
		}
		std::shared_ptr<FAC> factory = std::make_shared<FAC>(&domain.repository);
#if LOGGER_LIBRARY_AVAILABLE == 1
		if (m_logger) m_logger->info("Registering factory for " + std::string(factory->getName()));
#endif
		domain.factory = factory;
		return factory;
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
			for (auto& service : domain.services) {
				if (dynamic_cast<SER*>(service.get())) {
#if LOGGER_LIBRARY_AVAILABLE == 1
					if (m_logger) m_logger->error(std::string("Service: ") + typeid(SER).name() + " already exists in the model for the type: " + typeid(Model<Ts...>).name());
#endif
					return nullptr;
				}
			}

			std::shared_ptr<SER> service = std::make_shared<SER>(&domain.repository);
#if LOGGER_LIBRARY_AVAILABLE == 1
			if (m_logger) m_logger->info("Registering service: "+std::string(service->getName()));
#endif
			domain.services.push_back(service);
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
			if (m_logger) m_logger->info("Registering service: " + std::string(service->getName()));
#endif
			m_generalServices.push_back(service);
			return service;
		}
		return nullptr;
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromService SER>
	std::shared_ptr<ServiceExecutionResult> Model<Ts...>::executeService()
	{
		if constexpr ((std::is_same_v<typename SER::AggregateType, Ts> || ...))
		{
			AggregateContainer<typename SER::AggregateType>& domain = getAggregateContainer<typename SER::AggregateType>();
			for (auto& service : domain.services) {
				if (auto* ptr = dynamic_cast<SER*>(service.get())) {
#if LOGGER_LIBRARY_AVAILABLE == 1
					if (m_logger) m_logger->info("Executing service: " + std::string(ptr->getName()));
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
					if (m_logger) m_logger->info("Executing service: " + std::string(ptr->getName()));
#endif
					return ptr->execute();
				}
			}
		}
#if LOGGER_LIBRARY_AVAILABLE == 1
		if (m_logger) m_logger->error("Service: "+std::string(typeid(SER).name())+" not found for execution.\nService must be created first!");
#endif
		return nullptr;
	}



	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	[[nodiscard]] size_t Model<Ts...>::size() const
	{
		static_assert((std::is_same_v<AGG, Ts> || ...), "Aggregate type <AGG> not found in this model");
		const AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		return domain.repository.size();
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	[[nodiscard]] bool Model<Ts...>::empty() const
	{
		static_assert((std::is_same_v<AGG, Ts> || ...), "Aggregate type <AGG> not found in this model");
		const AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		return domain.repository.empty();
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	[[nodiscard]] bool Model<Ts...>::contains(ID id) const
	{
		static_assert((std::is_same_v<AGG, Ts> || ...), "Aggregate type <AGG> not found in this model");
		const AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		return domain.repository.contains(id);
	}

	template <DerivedFromAggregate... Ts>
	[[nodiscard]] bool Model<Ts...>::contains(ID id) const
	{
		for (auto& agg : m_domains)
		{
			bool found = std::visit([id](auto& obj) {
				return obj.repository.contains(id);
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
		return domain.repository.contains(aggregate.getID());
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	[[nodiscard]] bool Model<Ts...>::contains(const std::shared_ptr<typename AGG>& aggregate) const
	{
		static_assert((std::is_same_v<AGG, Ts> || ...), "Aggregate type <AGG> not found in this model");
		const AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		return domain.repository.contains(aggregate->getID());
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	[[nodiscard]] bool Model<Ts...>::contains(const AGG* aggregate) const
	{
		static_assert((std::is_same_v<AGG, Ts> || ...), "Aggregate type <AGG> not found in this model");
		const AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		return domain.repository.contains(aggregate->getID());
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	[[nodiscard]] std::shared_ptr<AGG> Model<Ts...>::getAggregate(const ID id)
	{
		static_assert((std::is_same_v<AGG, Ts> || ...), "Aggregate type <AGG> not found in this model");
		AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		return domain.repository.get(id);
	}
	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	[[nodiscard]] std::shared_ptr<const AGG> Model<Ts...>::getAggregate(const ID id) const
	{
		static_assert((std::is_same_v<AGG, Ts> || ...), "Aggregate type <AGG> not found in this model");
		const AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		return domain.repository.get(id);
	}

	template <DerivedFromAggregate... Ts>
	[[nodiscard]] std::shared_ptr<Aggregate> Model<Ts...>::getAggregate(const ID id)
	{
		std::shared_ptr<Aggregate> objPtr = nullptr;
		for (auto& agg : m_domains)
		{
			std::visit([&objPtr, id](auto& obj) {
				if (!obj.repository.contains(id))
					return;
				objPtr = obj.repository.get(id);
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
				if (!obj.repository.contains(id))
					return;
				objPtr = obj.repository.get(id);
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
				return obj.repository.remove(id);
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
		domain.repository.clear();
	}

	template <DerivedFromAggregate... Ts>
	void Model<Ts...>::clear()
	{
		for (auto& agg : m_domains)
		{
			std::visit([](auto& obj) {
				obj.repository.clear();
				}, agg);
		}
		m_idDomain.reset();
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
			if (!domain.repository.contains(id))
				continue;
			std::shared_ptr<AGG> obj = domain.repository.get(id);
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
			if (!domain.repository.contains(id))
				continue;
			std::shared_ptr<const AGG> obj = domain.repository.get(id);
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
					if(!obj.repository.contains(id))
						continue;
					std::shared_ptr<Aggregate> ins = obj.repository.get(id);
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
					if (!obj.repository.contains(id))
						continue;
					std::shared_ptr<const Aggregate> ins = obj.repository.get(id);
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
		return domain.repository.getAll();
	}

	template <DerivedFromAggregate... Ts>
	[[nodiscard]] std::vector<std::shared_ptr<Aggregate>> Model<Ts...>::getAggregates()
	{
		std::vector<std::shared_ptr<Aggregate>> objs;
		for (auto& agg : m_domains)
		{
			std::visit([&objs](auto& obj) {
				for (auto& ins : obj.repository.getAll())
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
				for (auto& ins : obj.repository.getAll())
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
		objs.reserve(domain.repository.size());
		for (const auto& it : domain.repository.getALL())
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
				std::vector<std::shared_ptr<Aggregate>> deleted = obj.repository.getDeleted();
				objs.insert(objs.end(), deleted.begin(), deleted.end());
				}, agg);
		}
		return objs;
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG> 
	[[nodiscard]] std::vector<std::shared_ptr<AGG>> Model<Ts...>::getDeletedAggregates()
	{
		AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		return domain.repository.getDeleted();
	}

	template <DerivedFromAggregate... Ts>
	[[nodiscard]] std::vector<std::shared_ptr<const Aggregate>> Model<Ts...>::getDeletedAggregates() const
	{
		std::vector<std::shared_ptr<const Aggregate>> objs;
		for (auto& agg : m_domains)
		{
			std::visit([&objs](auto& obj) {
				std::vector<std::shared_ptr<const Aggregate>> deleted = obj.repository.getDeleted();
				objs.insert(objs.end(), deleted.begin(), deleted.end());
				}, agg);
		}
		return objs;
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG> 
	[[nodiscard]] std::vector<std::shared_ptr<const AGG>> Model<Ts...>::getDeletedAggregates() const
	{
		AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		return domain.repository.getDeleted();
	}

	template <DerivedFromAggregate... Ts>
	void Model<Ts...>::clearDeletedAggregates()
	{
		for (auto& agg : m_domains)
		{
			std::visit([](auto& obj) {
				obj.repository.clearDeletedCache();
				}, agg);
		}
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG> 
	void Model<Ts...>::clearDeletedAggregates()
	{
		AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		domain.repository.clearDeletedCache();
	}



	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	[[nodiscard]] std::vector<ID> Model<Ts...>::getIDs() const
	{
		std::vector<ID> ids;
		const AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		std::vector<ID> subIds = domain.repository.getIDs();
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
				std::vector<ID> subIds = obj.repository.getIDs();
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
			if (domain.repository.contains(id))
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
			if (m_logger) m_logger->info("Detaching persistence layer");
#endif
			m_persistence = nullptr;
		}
#if LOGGER_LIBRARY_AVAILABLE == 1
		if (m_logger) m_logger->info("Attaching persistence layer");
#endif
		std::shared_ptr<PER> persistence = std::make_shared<PER>();
		m_persistence = persistence;
		return persistence;
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

	//
	// PRIVATE
	//

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