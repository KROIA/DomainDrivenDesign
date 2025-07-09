#pragma once
#include "DDD_base.h"
#include "Repository.h"
#include "utilities/UniqueIDDomain.h"

namespace DDD
{
	/*
	* @brief
	* Factory class to create new aggregates.
	* 
	* @details
	* Create a derived class from this factory and implement one ore more create() methodes.
	* The create() methodes should create a new instance of the aggregate and register it in the repository.
	* Example:
	* 
	*	class AnimalFactory : public DDD::AggregateFactory<Animal>
	*	{
	*	public:
	*		AnimalFactory(DDD::Repository<Animal>* repo)
	*			: AggregateFactory(repo)
	*		{}
	*		~AnimalFactory() {}
	*
	*		// Create a new instance of the aggregate and register it in the repository
	*		std::shared_ptr<Animal> createAggregate()
	*		{
	* 			// the registerInstance() method will register the new instance in the model
	*           // When the aggrigate gets instantiated with a id == 0, the model will generate a new unique id, since 0 is not a valid id
	*			// When the id is != 0, and the model contains already a aggregate with the same id, the aggregate gets replaced by the new one.	
	*			// THe aggregate id is globally unique in the model. Different aggregate types can't have the same id.
	*			return registerInstance(std::make_shared<Animal>());
	*		}
	*	protected:
	*
	*	private:
	*
	*	};
	* 
	*  The factory must be registered in the model:
	*  DDD::Model<Animal> model;
	*  std::shared_ptr<AnimalFactory> animalFactory = model.createFactory<AnimalFactory>();
	* 
	*  The factory can be used to create new instances of the aggregate:
	*  animalFactory->createAggregate();
	* 
	* 
	* 
	*/
	template <DerivedFromAggregate AGG>
	class AggregateFactory
	{
	public:
		typedef AGG AggregateType;

		AggregateFactory(Repository<AGG>* repo)
			: m_repository(repo)
#if LOGGER_LIBRARY_AVAILABLE == 1
			, m_logger(getAggregateName() + "Factory")
#endif
		{}
		void unregister()
		{
			m_repository = nullptr;
#if LOGGER_LIBRARY_AVAILABLE == 1
			setLoggerParentID(0);
#endif
		}
		static std::string getAggregateName()
		{
			std::string raw = typeid(AGG).name();
			auto it = raw.rfind(":");
			if (it != std::string::npos)
			{
				raw = raw.substr(it + 1);
			}
			it = raw.rfind("class ");
			if (it != std::string::npos)
			{
				raw = raw.substr(it + 6);
			}
			return raw;
		}

#if LOGGER_LIBRARY_AVAILABLE == 1
		/**
		 * @brief Gets called by the model during setup to attach this logger to the model as a child logger
		 * @param parent logger, comming from the model
		 */
		void setLoggerParentID(const Log::LoggerID &id)
		{
			m_logger.setParentID(id);
		}

		Log::LoggerID getLoggerID() const
		{
			return m_logger.getID();
		}
		Log::LoggerID getLoggerParentID() const
		{
			return m_logger.getParentID();
		}
#endif
	protected:
		std::shared_ptr<AGG> registerInstance(std::shared_ptr<AGG> agg)
		{
			if (m_repository)
			{
				if (m_repository->add(agg))
					return agg;
#if LOGGER_LIBRARY_AVAILABLE == 1
				error("registerInstance(): Can't register object");
#endif
			}
#if LOGGER_LIBRARY_AVAILABLE == 1
			error("registerInstance(): This factory does not have a repository reference set.");
#endif
			return nullptr;
		}

		/**
		 * @brief Helpoer function to log messages.
		 * @param msg
		 */
		void debug(const std::string& msg) const 
		{ 
#if LOGGER_LIBRARY_AVAILABLE == 1
			m_logger.debug(msg); 
#else
			DDD_UNUSED(msg);
#endif
		}
		void info(const std::string& msg) const 
		{ 
#if LOGGER_LIBRARY_AVAILABLE == 1
			m_logger.info(msg);
#else
			DDD_UNUSED(msg);
#endif
		}
		void warning(const std::string& msg) const 
		{
#if LOGGER_LIBRARY_AVAILABLE == 1
			m_logger.warning(msg); 
#else
			DDD_UNUSED(msg);
#endif
		}
		void error(const std::string& msg) const 
		{ 
#if LOGGER_LIBRARY_AVAILABLE == 1
			m_logger.error(msg); 
#else
			DDD_UNUSED(msg);
#endif
		}

	private:
		Repository<AGG>* m_repository;
#if LOGGER_LIBRARY_AVAILABLE == 1
		mutable Log::LogObject m_logger;
#endif
	};
}