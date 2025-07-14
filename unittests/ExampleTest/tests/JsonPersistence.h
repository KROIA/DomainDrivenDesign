#pragma once
#include "DDD.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QIODevice>

class AnimalFactory;
class CatFactory;

class JsonPersistence : public DDD::IPersistence
{
public:
	JsonPersistence()
		: IPersistence()
		, m_folderPath("Data/")
	{
		// Create Folder if not exist
		QDir dir;
		if (!dir.exists(m_folderPath.c_str()))
		{
			dir.mkpath(m_folderPath.c_str());
		}

	}

	void setFactories(std::shared_ptr<AnimalFactory> animalFactory,	std::shared_ptr<CatFactory> catFactory)
	{
		this->animalFactory = animalFactory;
		this->catFactory = catFactory;
	}

	/**
	 * @brief Save all aggregates to the persistence layer
	 *
	 * @return true if the operation was successful, false otherwise
	 */
	bool save() override
	{
		return false;
	}

	/**
	 * @brief Save the given aggregates to the persistence layer
	 * @param ids
	 * @return true if the operation was successful, false otherwise
	 */
	bool save(const std::vector<DDD::ID>& ids) override
	{
		return false;
	}

	/**
	 * @brief Save the given aggregate to the persistence layer
	 * @details Save only the given aggregate but not deleting the others
	 * @param id The id for the aggregate to save
	 * @return true if the operation was successful, false otherwise
	 */
	 /*bool save(DDD::ID id) override
	{
		return false;
	}*/

	/**
	 * @brief Remove the given aggregates from the persistance layer
	 *
	 * @param ids The aggregate ids to remove
	 * @return true if the operation was successful, false otherwise
	 */
	 /*bool remove(const std::vector<DDD::ID>& ids)  override
	{
		return false;
	}*/

	/**
	 * @brief Remove the given aggregate from the persistance layer
	 *
	 * @param id The id of the aggregate to remove
	 * @return true if the operation was successful, false otherwise
	 */
	 /*bool remove(DDD::ID aggregate)  override
	{
		return false;
	}*/

	/*bool removeOnFilesystem() override
	{
		return false;
	}*/

	/**
	 * @brief Load all aggregates from the persistence layer
	 * @warning Use a factory to instantiate a new object, passing the needed data to the constructor
	 *
	 * @return true if the operation was successful, false otherwise
	 */
	bool load() override
	{
		return false;
	}


	/**
	 * @brief Load the aggregate with the given id from the persistence layer
	 * @warning Use a factory to instantiate a new object, passing the needed data to the constructor
	 *
	 * @param id The id of the aggregate to load
	 * @return true if the operation was successful, false otherwise
	 */
	/*bool load(DDD::ID id) override
	{
		return false;
	}*/

	/**
	 * @brief Load the aggregates with the given ids from the persistence layer
	 * @warning Use a factory to instantiate a new object, passing the needed data to the constructor
	 *
	 * @param ids The ids of the aggregates to load
	 * @return true if the operation was successful, false otherwise
	 */
	 bool load(const std::vector<DDD::ID>& ids) override
	 {
		return false;
	 }

	/**
	 * @brief Update the given aggregates in the persistance layer
	 * @details The aggregates must already exist in the persistance layer in order to succeed.
	 *			Load the data from the persistance layer in to the object, overwriting the current data.
	 *
	 * @param aggregates The aggregates to update
	 * @return true if the operation was successful, false otherwise
	 */
	/*bool update(const std::vector<std::shared_ptr<DDD::Aggregate>>& aggregates) override
	{
		return false;
	}*/

	/**
	 * @brief Update the given aggregate in the persistance layer
	 * @details The aggregate must already exist in the persistance layer in order to succeed.
	 *			Load the data from the persistance layer in to the object, overwriting the current data.
	 *
	 * @param aggregate The aggregate to update
	 * @return true if the operation was successful, false otherwise
	 */
	/*bool update(std::shared_ptr<DDD::Aggregate> aggregate) override
	{
		DDD::ID id = aggregate->getID();
		QString fileName = QString::fromStdString(m_folderPath) + QString::fromStdString(DDD::IID::getIDString(id)) + ".json";
		QFile file(fileName);
		if (!file.open(QIODevice::WriteOnly))
		{
			return false;
		}
		return false;
	}*/

	/**
	 * @brief Clear the persistance layer
	 * @details Remove all aggregates from the persistance layer
	 *
	 * @return true if the operation was successful, false otherwise
	 */
	 /*bool clear() override
	{
		// Delete all files
		QDir dir(m_folderPath.c_str());
		QStringList filters;
		filters << "*.json";
		dir.setNameFilters(filters);
		QStringList files = dir.entryList();
		for (const auto& file : files)
		{
			dir.remove(file);
		}
		return true;
	}*/

	/**
	 * @brief Removes all files from the filesystem that are associated with the database
	 * @return true if the operation was successful, false otherwise
	 */
	 bool removeDatabase() override
	 {
		 return true;
	 }

	/**
	 * @brief Check if the persistance layer contains the aggregate with the given id
	 *
	 * @param id The id of the aggregate to check
	 * @return true if the aggregate exists in the persistance layer, false otherwise
	 */
	 /*bool contains(DDD::ID id) override
	{
		// Get the file list of json files
		QDir dir(m_folderPath.c_str());
		QStringList filters;
		filters << "*.json";
		dir.setNameFilters(filters);
		QStringList files = dir.entryList();

		// Check if the file with the id in the name exists
		for (const auto& file : files)
		{
			if (file.contains(QString::fromStdString(DDD::IID::getIDString(id))))
			{
				return true;
			}
		}
		return false;
	}*/

	/**
	 * @brief Get the number of aggregates in the persistance layer
	 *
	 * @return The number of aggregates in the persistance layer
	 */
	 /*size_t size() override
	{
		// Get the amount of json files in the folder
		QDir dir(m_folderPath.c_str());
		QStringList filters;
		filters << "*.json";
		dir.setNameFilters(filters);
		return dir.entryList().size();
	}*/

	 bool lock(const DDD::ID& id) override
	 {
		 return false;
	 }

	 bool unlock(const DDD::ID& id)override
	 {
		 return false;
	 }


	 bool isLocked(const DDD::ID& id) override
	 {
		 return false;
	 }


	 std::vector<std::shared_ptr<DDD::AggregateLock>> getLocks() override
	 {
		 return {};
	 }

	 std::shared_ptr<DDD::AggregateLock> getLock(const DDD::ID& id) override
	 {
		 return nullptr;
	 }


	 bool logOnUser(std::shared_ptr<DDD::User> user) override
	 {
		 return false;
	 }

	 bool logOffUser(std::shared_ptr<DDD::User> user) override
	 {
		 return false;
	 }

	 std::vector<std::shared_ptr<DDD::User>> getLoggedOnUsers() override
	 {
		 return {};
	 }

	private:
		std::string m_folderPath;
		std::shared_ptr<AnimalFactory> animalFactory;
		std::shared_ptr<CatFactory> catFactory;
};