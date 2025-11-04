#pragma once
#include "DDD_base.h"
#include <vector>
#include "model/Aggregate.h"
#include "model/MetadataContainer.h"



namespace DDD
{
	class AggregateLock;
	class User;
	class IPersistence
	{
	public:
		IPersistence() = default;
		IPersistence(const IPersistence&) = default;
		IPersistence(IPersistence&&) = default;
		virtual ~IPersistence() = default;

		/**
		 * @brief Save all aggregates to the persistence layer
		 * 
		 * @return true if the operation was successful, false otherwise
	     */
		virtual bool save() = 0;

		/**
		 * @brief Save the given aggregates to the persistence layer
		 * @param ids 
		 * @return true if the operation was successful, false otherwise
		 */
		virtual bool save(const std::vector<ID>& ids) = 0;

		/**
		 * @brief Save the given metadata to the persistence layer
		 * @param metadata
		 * @return true if the operation was successful, false otherwise
		 */
		virtual bool save(std::shared_ptr<MetadataContainer> metadata) = 0;

		/**
		 * @brief Removes all files from the filesystem that are associated with the database
		 * @return true if the operation was successful, false otherwise
		 */
		virtual bool removeDatabase() = 0;

		/**
		 * @brief Load all aggregates from the persistence layer
		 * @warning Use a factory to instantiate a new object, passing the needed data to the constructor
		 *
		 * @return true if the operation was successful, false otherwise
		 */
		virtual bool load() = 0;

		/**
		 * @brief Load the aggregates with the given ids from the persistence layer
		 * @warning Use a factory to instantiate a new object, passing the needed data to the constructor
		 *
		 * @param ids The ids of the aggregates to load
		 * @return true if the operation was successful, false otherwise
		 */
		virtual bool load(const std::vector<ID>& ids) = 0;

		/**
		 * @brief Load the metadata from the persistence layer
		 *
		 * @param metadata The metadata container to load the metadata into
		 * @return true if the operation was successful, false otherwise
		 */
		virtual bool load(std::shared_ptr<MetadataContainer> metadata) = 0;

		/**
		 * @brief Locks an aggregate with the given id.
		 * @param id 
		 * @return true if the operation was successful, false otherwise
		 */
		virtual bool lock(const ID& id) = 0;

		/**
		 * @brief Unlocks an aggregate with the given id.
		 * @param id 
		 * @return true if the operation was successful, false otherwise
		 */
		virtual bool unlock(const ID& id) = 0;

		/**
		 * @brief Tries to unlock an aggregate with the given id for the given user, 
		 *        if the aggregate is locked.
		 * @param id 
		 * @return true if the object is now unlocked, false otherwise
		 */
		virtual bool tryUnlockIfLocked(const ID& id) = 0;

		/**
		 * @brief Checks if a given aggregate is locked.
		 * @param id 
		 * @return true if the aggregate is locked, false otherwise
		 */
		virtual bool isLocked(const ID& id) = 0;

		/**
		 * @brief Gets all locks that are currently active.
		 * @return vector of currently active locks
		 */
		virtual std::vector<std::shared_ptr<AggregateLock>> getLocks() = 0;

		/**
		 * @brief Gets the lock for the aggregate with the given id.
		 * @param id 
		 * @return nullptr if the aggregate is not locked, otherwise a shared pointer to the lock object
		 */
		virtual std::shared_ptr<AggregateLock> getLock(const ID& id) = 0;

		/**
		 * @brief Registers a user to the current session.
		 * @param user 
		 * @return true if the user was successfully logged on, false otherwise
		 */
		virtual bool logOnUser(std::shared_ptr<User> user) = 0;

		/**
		 * @brief Logs off a user from the current session.
		 * @param user
		 * @return true if the user was successfully logged off, false otherwise
		 */
		virtual bool logOffUser(std::shared_ptr<User> user) = 0;

		/**
		 * @brief Gets all users that are currently logged on.
		 * @return vector of currently logged on users
		 */
		virtual std::vector<std::shared_ptr<User>> getLoggedOnUsers() = 0;


		/**
		 * @brief Locks the database for atomic access
		 * All other methodes of this interface that access the database must not lock the database if this methode was called.
		 * If this methode was not called, all methodes must lock the database for their operation.
		 * @return true if the database is locked, false otherwise
		 */
		virtual bool lockDatabase() = 0;

		/**
		 * @brief Unlocks the database
		 * @return true if the database is unlocked, false otherwise
		 */
		virtual bool unlockDatabase() = 0;

		virtual bool isDatabaseLocked() const = 0;
	};
}