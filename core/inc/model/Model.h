#pragma once
#include "DDD_base.h"
#include "Domain.h"
#include <variant>
#include <array>

namespace DDD
{
	template <typename... Ts>
	class Model
	{
		// Template to find the first type derived from the specified Base
		template <typename Base, typename... Ts>
		struct FindDerivedFrom;

		// Specialization to recursively find the first type derived from Base
		template <typename Base, typename T, typename... Ts>
		struct FindDerivedFrom<Base, T, Ts...>
		{
			using Type = std::conditional_t<
				std::is_base_of_v<Base, T>,
				T, // If T is derived from Base, return it
				typename FindDerivedFrom<Base, Ts...>::Type // Otherwise, continue searching
			>;
		};

		// Base case: No matching type found
		template <typename Base>
		struct FindDerivedFrom<Base>
		{
			using Type = void;
		};

		// Helper alias
		template <typename Base, typename... Ts>
		using FindDerivedFromT = typename FindDerivedFrom<Base, Ts...>::Type;

	public:
		static constexpr size_t aggregateTypeCount = sizeof...(Ts);

		// Default constructor initializes each instance
		Model() {
			initialize_domains(std::index_sequence_for<Ts...>{});
		}

		// Get the instance array
		[[nodiscard]] const auto& getDomains() const { return m_domains; }

		// Retrieve an instance of a specific type X
		template <typename D>
		[[nodiscard]] D& getDomain() {
			// Check if D exists in the variant array, otherwise static assert
			static_assert((std::is_same_v<D, Ts> || ...), "Domain type <D> not found in this model");

			for (auto& var : m_domains) {
				if (auto* ptr = std::get_if<D>(&var)) {
					return *ptr;
				}
			}
			throw std::runtime_error("Domain type not found in this model");
		}


		template <typename D>
		ID executeFactoryCreateInstance(const ID factoryID, std::shared_ptr<FactoryCreationData> factoryData)
		{
			static_assert((std::is_same_v<D, Ts> || ...), "Domain type <D> not found in this model");
			D& domain = getDomain<D>();
			return domain.executeFactoryCreateInstance(factoryID, factoryData);
		}

		template <typename D>
		void executeFactoryRemoveInstance(ID factoryID, ID aggregateID)
		{
			static_assert((std::is_same_v<D, Ts> || ...), "Domain type <D> not found in this model");
			D& domain = getDomain<D>();
			domain.executeFactoryRemoveInstance(factoryID, aggregateID);
		}
		template <typename D>
		void executeFactoryReplaceInstance(ID factoryID, std::shared_ptr<FactoryCreationData> factoryData)
		{
			static_assert((std::is_same_v<D, Ts> || ...), "Domain type <D> not found in this model");
			D& domain = getDomain<D>();
			domain.executeFactoryReplaceInstance(factoryID, factoryData);
		}

		template <typename D>
		std::shared_ptr<ServiceExecutionResult> executeService(const ID id)
		{
			static_assert((std::is_same_v<D, Ts> || ...), "Domain type <D> not found in this model");
			D& domain = getDomain<D>();
			return domain.executeService(id);
		}




		template <typename D>
		[[nodiscard]] size_t size() const
		{
			static_assert((std::is_same_v<D, Ts> || ...), "Domain type <D> not found in this model");
			D& domain = getDomain<D>();
			return domain.size();
		}
		template <typename D>
		[[nodiscard]] bool empty() const
		{
			static_assert((std::is_same_v<D, Ts> || ...), "Domain type <D> not found in this model");
			D& domain = getDomain<D>();
			return domain.empty();
		}
		template <typename D>
		[[nodiscard]] bool contains(ID id) const
		{
			static_assert((std::is_same_v<D, Ts> || ...), "Domain type <D> not found in this model");
			D& domain = getDomain<D>();
			return domain.contains(id);
		}
		template <typename D>
		[[nodiscard]] bool contains(const D::AggregateType& aggregate) const
		{
			static_assert((std::is_same_v<D, Ts> || ...), "Domain type <D> not found in this model");
			D& domain = getDomain<D>();
			return domain.contains(aggregate.id());
		}
		template <typename D>
		[[nodiscard]] bool contains(const std::shared_ptr<typename D::AggregateType>& aggregate) const
		{
			static_assert((std::is_same_v<D, Ts> || ...), "Domain type <D> not found in this model");
			D& domain = getDomain<D>();
			return domain.contains(aggregate->id());
		}
		template <typename D>
		[[nodiscard]] bool contains(const D::AggregateType* aggregate) const
		{
			static_assert((std::is_same_v<D, Ts> || ...), "Domain type <D> not found in this model");
			D& domain = getDomain<D>();
			return domain.contains(aggregate->id());
		}

	private:
		// Helper function to initialize each element in the array
		template <std::size_t... I>
		constexpr void initialize_domains(std::index_sequence<I...>) {
			((m_domains[I].template emplace<Ts>()), ...);
		}
		
		using VariantType = std::variant<Ts...>;
		std::array<VariantType, aggregateTypeCount> m_domains; // Store instances in a variant array
	};
}