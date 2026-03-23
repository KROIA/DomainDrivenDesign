#pragma once
#include "DDD_base.h"
#include <vector>
#include <QList>
#include "model/Aggregate.h"
#include "utilities/IStringifyable.h"
#include "utilities/IDebugJsonObject.h"

namespace DDD
{
	/**
	 * @brief This class represents the result of a validation process.
	 * A result can contain additional sub-results, creating a tree structure of validation results.
	 */
	class DDD_API ValidationResult : public IDebugJsonObject, public IStringifyable
	{
	public:
		enum class Status
		{
			Valid,
			Invalid
		};
		ValidationResult(const std::string& title);
		ValidationResult(const ValidationResult& other);
		ValidationResult(ValidationResult&& other) noexcept;
		ValidationResult(const std::string& title, const std::vector<ValidationResult>& subResults);

		virtual ~ValidationResult() = default;

		ValidationResult& operator=(const ValidationResult& other);
		ValidationResult& operator=(ValidationResult&& other) noexcept;



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

		void addMessage(const std::string& message);
		void addMessage(std::string&& message);
		const std::vector<std::string>& getMessages() const
		{
			return m_messages;
		}
		void clearMessages();
		void clearSubResults();
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
		void invalidate(const std::string& message = "");

		const std::vector<ValidationResult>& getSubResults() const
		{
			return m_subResults;
		}
		const ValidationResult& getSubResult(size_t index) const;
		const ValidationResult& operator[](size_t index) const;
		const ValidationResult& front() const;
		const ValidationResult& back() const;
		Status addSubResult(const ValidationResult& subResult);
		Status addSubResult(const std::vector<ValidationResult>& subResults);
		Status removeSubResult(size_t index);

		QJsonObject toDebugJsonObject() const override;
		std::string toString() const;
		QString getTreeViewString() const;


		ValidationResult getReduced(Status keep) const;

	private:
		void buildTreeViewRecursive(QList<QString>& lines, int depth) const;

		// Helper function to split string by newline
		std::vector<QString> splitByNewline(const QString& str) const;

		std::string m_title;
		Status m_status{ Status::Valid };
		std::vector<std::string> m_messages;


		std::vector<ValidationResult> m_subResults;
	};
}