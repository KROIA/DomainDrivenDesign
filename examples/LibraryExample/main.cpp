#ifdef QT_ENABLED
#include <QApplication>
#endif
#include <iostream>
#include "DDD.h"

#ifdef QT_WIDGETS_ENABLED
#include <QWidget>
#endif


class Leg : public DDD::Entity
{
public:
	Leg(DDD::ID id)
		: Entity(id)
	{}
	~Leg() {}
};

class Person : public DDD::Aggregate
{
public: 
	Person(DDD::ID id)
		: Aggregate(id)
	{
		m_leg = std::make_shared<Leg>(1);
		addEntity(m_leg);
	}
	~Person() {}

private:
	std::shared_ptr<Leg> m_leg;
};

class Animal : public DDD::Aggregate
{
public:
	Animal(DDD::ID id)
		: Aggregate(id)
	{
		m_leg = std::make_shared<Leg>(1);
		addEntity(m_leg);
	}
	~Animal() {}

private:
	std::shared_ptr<Leg> m_leg;
};

class PersonFactory : public DDD::AggregateFactory<Person>
{
public:
	PersonFactory(DDD::Repository<Person>& repo, DDD::ID id)
		: AggregateFactory(repo, id)
	{}
	~PersonFactory() {}
protected:
	std::shared_ptr<Person> createAggregate(std::shared_ptr<DDD::FactoryCreationData> data) override
	{
		return std::make_shared<Person>(++m_idCounter);
	}
private:
	DDD::ID m_idCounter = 0;
};
class AnimalFactory : public DDD::AggregateFactory<Animal>
{
public:
	AnimalFactory(DDD::Repository<Animal>& repo, DDD::ID id)
		: AggregateFactory(repo, id)
	{}
	~AnimalFactory() {}
protected:
	std::shared_ptr<Animal> createAggregate(std::shared_ptr<DDD::FactoryCreationData> data) override
	{
		return std::make_shared<Animal>(++m_idCounter);
	}
private:
	DDD::ID m_idCounter = 0;
};

class PersonService : public DDD::Service<Person>
{
public:
	PersonService(DDD::ID id)
		: Service(id)
	{}
	~PersonService() {}

protected:
	std::shared_ptr<DDD::ServiceExecutionResult> execute(const std::unordered_map<DDD::ID, std::shared_ptr<Person>>& repo) override
	{
		std::cout << "PersonService::execute " << repo.size() << std::endl;
		return std::make_shared<DDD::ServiceExecutionResult>();
	}
};
class AnimalService : public DDD::Service<Animal>
{
public:
	AnimalService(DDD::ID id)
		: Service(id)
	{}
	~AnimalService() {}

protected:
	std::shared_ptr<DDD::ServiceExecutionResult> execute(const std::unordered_map<DDD::ID, std::shared_ptr<Animal>>& repo) override
	{
		std::cout << "AnimalService::execute " << repo.size() << std::endl;
		return std::make_shared<DDD::ServiceExecutionResult>();
	}
};

class PersonDomain : public DDD::Domain<Person>
{
public:
	PersonDomain()
		: Domain()
	{
		//std::shared_ptr<PersonService> personService = std::make_shared<PersonService>(1);
		//addService(personService);
		createService<PersonService>(1);
		createFactory<PersonFactory>(1);
	}
	~PersonDomain() {}
};
class AnimalDomain : public DDD::Domain<Animal>
{
public:
	AnimalDomain()
		: Domain()
	{
		createService<AnimalService>(1);
		createFactory<AnimalFactory>(1); 
	}
	~AnimalDomain() {}
};




int main(int argc, char* argv[]){
#ifdef QT_WIDGETS_ENABLED
	QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
	QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
#ifdef QT_ENABLED
	QApplication app(argc, argv);
#endif
	DDD::Profiler::start();
	DDD::LibraryInfo::printInfo();
#ifdef QT_WIDGETS_ENABLED
	QWidget* widget = DDD::LibraryInfo::createInfoWidget();
	if (widget)
		widget->show();
#endif


	DDD::Model<PersonDomain, AnimalDomain> model;
	DDD::ID personID = model.executeFactoryCreateInstance<PersonDomain>(1, nullptr);
	DDD::ID animalID = model.executeFactoryCreateInstance<AnimalDomain>(1, nullptr);

	std::shared_ptr<DDD::ServiceExecutionResult> personResult = model.executeService<PersonDomain>(1);
	std::shared_ptr<DDD::ServiceExecutionResult> animalResult = model.executeService<AnimalDomain>(1);

	//personDomain.executeFactoryRemoveInstance(personID, personID);


	//auto instances = model.getDomains();

	

	int ret = 0;
#ifdef QT_ENABLED
	ret = app.exec();
#endif
	DDD::Profiler::stop((std::string(DDD::LibraryInfo::name) + ".prof").c_str());
	return ret;
}