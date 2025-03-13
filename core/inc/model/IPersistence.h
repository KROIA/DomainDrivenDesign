#pragma once
#include "DDD_base.h"
#include <vector>
#include "model/Aggregate.h"



namespace DDD
{
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
		 * @brief Save the given aggregate to the persistence layer
		 * @details Save only the given aggregate but not deleting the others
		 * @param id The id for the aggregate to save
		 * @return true if the operation was successful, false otherwise
		 */
		virtual bool save(ID id) = 0;

		/**
		 * @brief Remove the given aggregates from the persistence layer
		 * 
		 * @param ids The aggregate ids to remove
		 * @return true if the operation was successful, false otherwise
		 */
		virtual bool remove(const std::vector<ID>& ids) = 0;

		/**
		 * @brief Remove the given aggregate from the persistence layer
		 *
		 * @param id The id of the aggregate to remove
		 * @return true if the operation was successful, false otherwise
		 */
		virtual bool remove(ID id) = 0;

		/**
		 * @brief Load all aggregates from the persistence layer
		 * @warning Use a factory to instantiate a new object, passing the needed data to the constructor
		 *
		 * @return true if the operation was successful, false otherwise
		 */
		virtual bool load() = 0;

		/**
		 * @brief Load the aggregate with the given id from the persistence layer
		 * @warning Use a factory to instantiate a new object, passing the needed data to the constructor
		 * 
		 * @param id The id of the aggregate to load
		 * @return true if the operation was successful, false otherwise
		 */
		virtual bool load(ID id) = 0;

		/**
		 * @brief Load the aggregates with the given ids from the persistence layer
		 * @warning Use a factory to instantiate a new object, passing the needed data to the constructor
		 *
		 * @param ids The ids of the aggregates to load
		 * @return true if the operation was successful, false otherwise
		 */
		virtual bool load(const std::vector<ID>& ids) = 0;

		/**
		 * @brief Update the given aggregates in the persistence layer
		 * @details The aggregates must already exist in the persistence layer in order to succeed.
		 *			Load the data from the persistence layer in to the object, overwriting the current data.
		 *
		 * @param aggregates The aggregates to update
		 * @return true if the operation was successful, false otherwise
		 */ 
		//virtual bool update(const std::vector<ID>& aggregates) = 0;

		/**
		 * @brief Update the given aggregate in the persistence layer
		 * @details The aggregate must already exist in the persistence layer in order to succeed.
		 *			Load the data from the persistence layer in to the object, overwriting the current data.
		 *
		 * @param aggregate The aggregate to update
		 * @return true if the operation was successful, false otherwise
		 */
		//virtual bool update(std::shared_ptr<Aggregate> aggregate) = 0;

		/**
		 * @brief Clear the persistence layer
		 * @details Remove all aggregates from the persistence layer
		 * 
		 * @return true if the operation was successful, false otherwise
		 */
		virtual bool clear() = 0;

		/**
		 * @brief Check if the persistence layer contains the aggregate with the given id
		 *
		 * @param id The id of the aggregate to check
		 * @return true if the aggregate exists in the persistence layer, false otherwise
		 */
		virtual bool contains(DDD::ID id) = 0;

		/**
		 * @brief Get the number of aggregates in the persistence layer
		 * 
		 * @return The number of aggregates in the persistence layer
		 */
		virtual size_t size() = 0;
	};
}