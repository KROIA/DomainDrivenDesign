#pragma once
#include "DDD_base.h"
#include <vector>
#include "model/Aggregate.h"
#include "IJsonSerializable.h"


namespace DDD
{
	/**
	 * @brief This class represents the result of a validation process.
	 * A result can contain additional sub-results, creating a tree structure of validation results.
	 */
	class ValidationResult : public IJsonSerializable
	{
	public:
		enum class Status
		{
			Valid,
			Invalid
		};
		ValidationResult(const std::string &title)
			: m_title(title)
			, m_status(Status::Valid)
		{}
		ValidationResult(const ValidationResult& other) 
			: m_title(other.m_title)
			, m_status(other.m_status)
			, m_messages(other.m_messages)
			, m_subResults(other.m_subResults)
		{}
		ValidationResult(ValidationResult&& other) noexcept
			: m_title(std::move(other.m_title))
			, m_status(std::move(other.m_status))
			, m_messages(std::move(other.m_messages))
			, m_subResults(std::move(other.m_subResults))
		{}

		virtual ~ValidationResult() = default;

		ValidationResult& operator=(const ValidationResult& other)
		{
			if (this != &other)
			{
				m_title = other.m_title;
				m_status = other.m_status;
				m_messages = other.m_messages;
				m_subResults = other.m_subResults;
			}
			return *this;
		}
		ValidationResult& operator=(ValidationResult&& other) noexcept
		{
			if (this != &other)
			{
				m_title = std::move(other.m_title);
				m_status = std::move(other.m_status);
				m_messages = std::move(other.m_messages);
				m_subResults = std::move(other.m_subResults);
			}
			return *this;
		}



		Status getStatus() const
		{
			return m_status;
		}
		void setStatus(Status status)
		{
			m_status = status;
		}

		const std::string& getTitle() const
		{
			return m_title;
		}

		void addMessage(const std::string& message)
		{
			m_messages.push_back(message);
		}
		void addMessage(std::string&& message)
		{
			m_messages.push_back(std::move(message));
		}
		const std::vector<std::string>& getMessages() const
		{
			return m_messages;
		}
		void clearMessages()
		{
			m_messages.clear();
		}
		void clearSubResults()
		{
			m_subResults.clear();
		}
		bool isValid() const
		{
			return m_status == Status::Valid;
		}
		bool isInvalid() const
		{
			return m_status == Status::Invalid;
		}
		void setValid()
		{
			m_status = Status::Valid;
		}
		void setInvalid()
		{
			m_status = Status::Invalid;
		}
		void invalidate(const std::string& message = "")
		{
			if (!message.empty())
			{
				addMessage(message);
			}
			m_status = Status::Invalid;
		}

		const std::vector<ValidationResult>& getSubResults() const
		{
			return m_subResults;
		}
		void addSubResult(const ValidationResult& subResult)
		{
			if(!subResult.isValid())
			{
				m_status = Status::Invalid;
			}
			m_subResults.push_back(subResult);
		}
		void removeSubResult(size_t index)
		{
			if (index < m_subResults.size())
			{
				m_subResults.erase(m_subResults.begin() + index);
			}
		}

		QJsonObject toJson() const override
		{
			QJsonObject json;
			switch (m_status)
			{
			case Status::Valid:
				json["status"] = "Valid";
				break;
			case Status::Invalid:
			default:
				json["status"] = "Invalid";
				break;
			}
			if (!m_messages.empty())
			{
				QJsonArray messagesArray;
				for (const auto& message : m_messages)
				{
					messagesArray.append(QString::fromStdString(message));
				}
				json["messages"] = messagesArray;
			}
			json["title"] = QString::fromStdString(m_title);

			if (!m_subResults.empty())
			{
				QJsonArray subResultsArray;
				for (const auto& subResult : m_subResults)
				{
					subResultsArray.append(subResult.toJson());
				}
				json["subResults"] = subResultsArray;
			}
			return json;
		}
		std::string toString() const
		{
			return jsonToString(toJson());
		}


		

	private:
		std::string m_title;
		Status m_status{ Status::Valid };
		std::vector<std::string> m_messages;


		std::vector<ValidationResult> m_subResults;
	};
}