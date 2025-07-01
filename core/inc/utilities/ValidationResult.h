#pragma once
#include "DDD_base.h"
#include <vector>
#include "model/Aggregate.h"
#include "IJsonSerializable.h"


namespace DDD
{
	class DDD_EXPORT ValidationResult : public IJsonSerializable
	{
	public:
		enum class Status
		{
			Valid,
			Invalid,
			Warning
		};
		ValidationResult() = default;
		ValidationResult(const ValidationResult& other) 
			: m_status(other.m_status)
			, m_messages(other.m_messages)
		{}
		ValidationResult(ValidationResult&& other) noexcept
			: m_status(std::move(other.m_status))
			, m_messages(std::move(other.m_messages))
		{}


		Status getStatus() const
		{
			return m_status;
		}
		void setStatus(Status status)
		{
			m_status = status;
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
		bool isValid() const
		{
			return m_status == Status::Valid;
		}
		bool isInvalid() const
		{
			return m_status == Status::Invalid;
		}
		bool isWarning() const
		{
			return m_status == Status::Warning;
		}
		void setValid()
		{
			m_status = Status::Valid;
			clearMessages();
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
				json["status"] = "Invalid";
				break;
			case Status::Warning:
				json["status"] = "Warning";
				break;
			default:
				json["status"] = "Unknown";
				break;
			}
			QJsonArray messagesArray;
			for (const auto& message : m_messages)
			{
				messagesArray.append(QString::fromStdString(message));
			}
			json["messages"] = messagesArray;
			return json;
		}
		std::string toString() const
		{
			return jsonToString(toJson());
		}

	private:
		Status m_status{ Status::Valid };
		std::vector<std::string> m_messages;
	};
}