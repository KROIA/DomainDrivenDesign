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
	public:
		static constexpr size_t size = sizeof...(Ts);

		// Default constructor initializes each instance
		Model() {
			initialize_domains(std::index_sequence_for<Ts...>{});
		}

		// Get the instance array
		const auto& getDomains() const { return m_domains; }

		// Retrieve an instance of a specific type X
		template <typename D>
		D& getDomain() {
			// Check if D exists in the variant array, otherwise static assert
			static_assert((std::is_same_v<D, Ts> || ...), "Domain type <D> not found in instances");

			for (auto& var : m_domains) {
				if (auto* ptr = std::get_if<D>(&var)) {
					return *ptr;
				}
			}
			throw std::runtime_error("Domain type not found in instances");
		}

		template <typename D>
		std::shared_ptr<ServiceExecutionResult> executeService(ID id)
		{
			static_assert((std::is_same_v<D, Ts> || ...), "Domain type <D> not found in instances");
			D& domain = getDomain<D>();
			return domain.executeService(id);
		}

	private:
		// Helper function to initialize each element in the array
		template <std::size_t... I>
		constexpr void initialize_domains(std::index_sequence<I...>) {
			((m_domains[I] = Ts{}), ...);
		}
		
		using VariantType = std::variant<Ts...>;
		std::array<VariantType, sizeof...(Ts)> m_domains; // Store instances in a variant array
	};
}