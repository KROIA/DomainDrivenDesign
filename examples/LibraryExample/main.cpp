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

class Person2 : public DDD::Aggregate
{
public:
	Person2(DDD::ID id)
		: Aggregate(id)
	{
		m_leg = std::make_shared<Leg>(1);
		addEntity(m_leg);
	}
	~Person2() {}

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

class PersonDomain : public DDD::Domain<Person>
{
public:
	PersonDomain()
		: Domain()
	{
		std::shared_ptr<PersonService> personService = std::make_shared<PersonService>(1);
		addService(personService);
	}
	~PersonDomain() {}
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

	DDD::Model<PersonDomain> model;
	PersonDomain& personDomain = model.getDomain<PersonDomain>();
	DDD::ID facID = personDomain.createFactory<PersonFactory>();
	DDD::ID personID = personDomain.executeFactoryCreateInstance(facID, nullptr);

	std::shared_ptr<DDD::ServiceExecutionResult> result = model.executeService<PersonDomain>(1);

	personDomain.executeFactoryRemoveInstance(facID, personID);

	std::shared_ptr<DDD::ServiceExecutionResult> result2 = model.executeService<PersonDomain>(1);


	auto instances = model.getDomains();

	

	int ret = 0;
#ifdef QT_ENABLED
	ret = app.exec();
#endif
	DDD::Profiler::stop((std::string(DDD::LibraryInfo::name) + ".prof").c_str());
	return ret;
}