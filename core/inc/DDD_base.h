#pragma once

/// USER_SECTION_START 1

/// USER_SECTION_END

#include "DDD_global.h"
#include "DDD_debug.h"
#include "DDD_info.h"

/// USER_SECTION_START 2
namespace DDD
{
	typedef size_t ID;
	const ID INVALID_ID = 0;

	class Entity;
	class Aggregate;
	class Service;


	template <typename T>
	concept DerivedFromAggregate = std::is_base_of_v<Aggregate, T>;

	template <typename T>
	concept DerivedFromEntity = std::is_base_of_v<Entity, T>;

	template <typename T>
	concept DerivedFromService = std::is_base_of_v<Service, T>;

	template <DerivedFromAggregate AGG>
	class AggregateFactory;

	//template <typename T, typename AGG>
	//concept DerivedFromAggregateFactory = std::is_base_of_v<AggregateFactory<AGG>, T>;
}



/// USER_SECTION_END