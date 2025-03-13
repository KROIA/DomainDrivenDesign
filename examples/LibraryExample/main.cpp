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
	Person()
		: Aggregate()
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
	Animal()
		: Aggregate()
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
	PersonFactory(DDD::Repository<Person>* repo)
		: AggregateFactory(repo)
	{}
	~PersonFactory() {}

	std::shared_ptr<Person> createAggregate()
	{
		return registerInstance(std::make_shared<Person>());
	}
protected:
	
private:

};
class AnimalFactory : public DDD::AggregateFactory<Animal>
{
public:
	AnimalFactory(DDD::Repository<Animal>* repo)
		: AggregateFactory(repo)
	{}
	~AnimalFactory() {}

	std::shared_ptr<Animal> createAggregate()
	{
		return registerInstance(std::make_shared<Animal>());
	}
protected:
	
private:

};

class PersonService : public DDD::AggregateService<Person>
{
public:
	PersonService(DDD::Repository<Person>* repository)
		: AggregateService(repository)
	{}
	~PersonService() {}
	std::shared_ptr<DDD::ServiceExecutionResult> execute() override
	{
		std::cout << "PersonService::execute " << getRepository()->size() << std::endl;
		return std::make_shared<DDD::ServiceExecutionResult>();
	}

protected:
	
};
class AnimalService : public DDD::AggregateService<Animal>
{
public:
	AnimalService(DDD::Repository<Animal>* repository)
		: AggregateService(repository)
	{}
	~AnimalService() {}

	std::shared_ptr<DDD::ServiceExecutionResult> execute() override
	{
		std::cout << "AnimalService::execute " << getRepository()->size() << std::endl;
		return std::make_shared<DDD::ServiceExecutionResult>();
	}
protected:
	
};



DDD::Model<Person, Animal> model;

class GeneralService : public DDD::Service
{
public:
	GeneralService()
		: Service("GeneralService")
	{}
	~GeneralService() {}

	std::shared_ptr<DDD::ServiceExecutionResult> execute() override
	{
		std::cout << "GeneralService::execute " << std::endl;
		std::vector<DDD::ID> ids = model.getIDs();
		for (size_t i = 0; i < ids.size(); ++i)
			std::cout << "ID: " << ids[i] << "\n";

		ids = model.getIDs<Person>();
		for (size_t i = 0; i < ids.size(); ++i)
			std::cout << "ID: " << ids[i] << "\n";
	//	std::cout << model.getAggregate(3)->getID() << std::endl;

		return std::make_shared<DDD::ServiceExecutionResult>();
	}
protected:

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

	Log::UI::NativeConsoleView::createStaticInstance();
	Log::UI::NativeConsoleView::getStaticInstance()->show();
	

	std::shared_ptr<PersonFactory> personFactory = model.createFactory<PersonFactory>();
	std::shared_ptr<AnimalFactory> animalFactory = model.createFactory<AnimalFactory>();

	model.createService<PersonService>();
	model.createService<AnimalService>();
	model.createService<GeneralService>();

	auto person = personFactory->createAggregate();
	auto animal = animalFactory->createAggregate();


	//
	std::shared_ptr<DDD::ServiceExecutionResult> personResult = model.executeService<PersonService>();
	std::shared_ptr<DDD::ServiceExecutionResult> generalResult = model.executeService<GeneralService>();

	auto person1 = model.getAggregate(person->getID());
	if (person1)
	{
		person1->markDeleted();
	}
	std::shared_ptr<DDD::ServiceExecutionResult> personResult2 = model.executeService<PersonService>();
	std::shared_ptr<DDD::ServiceExecutionResult> generalResult2 = model.executeService<GeneralService>();
	//std::shared_ptr<DDD::ServiceExecutionResult> animalResult = model.executeService<AnimalDomain>(1);

	//personDomain.executeFactoryRemoveInstance(personID, personID);


	//auto instances = model.getDomains();

	

	int ret = 0;
#ifdef QT_ENABLED
	ret = app.exec();
#endif
	DDD::Profiler::stop((std::string(DDD::LibraryInfo::name) + ".prof").c_str());
	return ret;
}