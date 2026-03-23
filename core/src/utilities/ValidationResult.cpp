#include "utilities/ValidationResult.h"
#include <QJsonArray>

namespace DDD
{


	ValidationResult::ValidationResult(const std::string& title)
		: m_title(title)
		, m_status(Status::Valid)
	{
	}
	ValidationResult::ValidationResult(const ValidationResult& other)
		: m_title(other.m_title)
		, m_status(other.m_status)
		, m_messages(other.m_messages)
		, m_subResults(other.m_subResults)
	{
	}
	ValidationResult::ValidationResult(ValidationResult&& other) noexcept
		: m_title(std::move(other.m_title))
		, m_status(std::move(other.m_status))
		, m_messages(std::move(other.m_messages))
		, m_subResults(std::move(other.m_subResults))
	{
	}
	ValidationResult::ValidationResult(const std::string& title, const std::vector<ValidationResult>& subResults)
		: m_title(title)
		, m_status(Status::Valid)
		, m_subResults(subResults)
	{
		for (const auto& subResult : subResults)
		{
			if (!subResult.isValid())
			{
				m_status = Status::Invalid;
			}
		}
	}

	ValidationResult& ValidationResult::operator=(const ValidationResult& other)
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
	ValidationResult& ValidationResult::operator=(ValidationResult&& other) noexcept
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

	void ValidationResult::addMessage(const std::string& message)
	{
		m_messages.push_back(message);
	}
	void ValidationResult::addMessage(std::string&& message)
	{
		m_messages.push_back(std::move(message));
	}

	void ValidationResult::clearMessages()
	{
		m_messages.clear();
	}
	void ValidationResult::clearSubResults()
	{
		m_subResults.clear();
	}

	void ValidationResult::invalidate(const std::string& message)
	{
		if (!message.empty())
		{
			addMessage(message);
		}
		m_status = Status::Invalid;
	}


	const ValidationResult& ValidationResult::getSubResult(size_t index) const
	{
		return m_subResults.at(index);
	}
	const ValidationResult& ValidationResult::operator[](size_t index) const
	{
		return m_subResults.at(index);
	}
	const ValidationResult& ValidationResult::front() const
	{
		return m_subResults.front();
	}
	const ValidationResult& ValidationResult::back() const
	{
		return m_subResults.back();
	}
	ValidationResult::Status ValidationResult::addSubResult(const ValidationResult& subResult)
	{
		if (!subResult.isValid())
		{
			m_status = Status::Invalid;
		}
		m_subResults.push_back(subResult);
		return m_status;
	}
	ValidationResult::Status ValidationResult::addSubResult(const std::vector<ValidationResult>& subResults)
	{
		for (const auto& subResult : subResults)
		{
			addSubResult(subResult);
		}
		return m_status;
	}
	ValidationResult::Status ValidationResult::removeSubResult(size_t index)
	{
		if (index < m_subResults.size())
		{
			m_subResults.erase(m_subResults.begin() + index);
		}
		m_status = Status::Valid;
		for (size_t i = 0; i < m_subResults.size(); ++i)
		{
			if (m_subResults[i].isInvalid())
			{
				m_status = Status::Invalid;
				return m_status;
			}
		}
		return m_status;
	}

	QJsonObject ValidationResult::toDebugJsonObject() const
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
				subResultsArray.append(subResult.toDebugJsonObject());
			}
			json["subResults"] = subResultsArray;
		}
		return json;
	}
	std::string ValidationResult::toString() const
	{
		return jsonToString(toDebugJsonObject());
	}
	QString ValidationResult::getTreeViewString() const
	{
		int tabs = 0;
		QList<QString> lines;
		buildTreeViewRecursive(lines, tabs);
		return lines.join("\n");
	}

	ValidationResult ValidationResult::getReduced(Status keep) const
	{
		ValidationResult reduced(m_title);
		reduced.m_status = m_status;
		if (m_status == keep)
		{
			reduced.m_messages = m_messages;
		}
		for (const auto& subResult : m_subResults)
		{
			if (subResult.m_status == keep)
			{
				reduced.m_subResults.push_back(subResult.getReduced(keep));
			}
		}
		return reduced;
	}


	void ValidationResult::buildTreeViewRecursive(QList<QString>& lines, int depth) const
	{
		// Build prefix for current depth
		QString prefix;
		for (int i = 0; i < depth; i++)
		{
			prefix += " ";
		}

		// Add title if at root level (depth == 0)
		if (depth == 0)
		{
			lines.push_back(QString::fromStdString(m_title + " : " + (isValid() ? "Valid" : "Invalid")));
		}

		// Calculate total items (texts + children)
		size_t totalItems = m_messages.size() + m_subResults.size();
		size_t currentItem = 0;

		// Add all texts first
		for (const auto& text : m_messages)
		{
			currentItem++;
			bool isLast = (currentItem == totalItems);

			QString branch = QString::fromUtf16(isLast ? u" └ " : u" ├ ");
			QString continuation = QString::fromUtf16(isLast ? u"   " : u" │ ");

			// Split text by newlines
			QString allLine = QString::fromStdString(text);
			std::vector<QString> textLines = splitByNewline(allLine);

			for (size_t i = 0; i < textLines.size(); i++)
			{
				if (i == 0)
				{
					// First line uses the branch character
					lines.push_back(prefix + branch + textLines[i]);
				}
				else
				{
					// Continuation lines are indented with continuation character
					lines.push_back(prefix + continuation + textLines[i]);
				}
			}

			//QString branch = QString::fromUtf16(isLast ? u" └ " : u" ├ ");
			//lines.push_back(prefix + branch + QString::fromStdString(text));
		}

		// Add all children recursively
		for (size_t i = 0; i < m_subResults.size(); i++)
		{
			currentItem++;
			bool isLast = (currentItem == totalItems);

			QString branch = QString::fromUtf16(isLast ? u" └ " : u" ├ ");
			lines.push_back(prefix + branch + QString::fromStdString(m_subResults[i].m_title + " : " + (m_subResults[i].isValid() ? "Valid" : "Invalid")));

			// Build continuation prefix for child's content
			QString childPrefix = prefix + QString::fromUtf16(isLast ? u"  " : u" │");

			// Recursively process child with modified prefix handling
			QList<QString> childLines;
			m_subResults[i].buildTreeViewRecursive(childLines, depth + 1); // Start child at depth 1

			// Add child lines with proper prefix
			for (size_t j = 0; j < childLines.size(); j++)
			{
				// Skip the first line (title) as we already added it
				//if (j > 0)
				//{
				lines.push_back(childPrefix + childLines[j]);
				//}
			}
		}
	}

	// Helper function to split string by newline
	std::vector<QString> ValidationResult::splitByNewline(const QString& str) const
	{
		std::vector<QString> result;
		QString current;

		for (const auto c : str)
		{
			if (c == '\n')
			{
				result.push_back(current);
				current.clear();
			}
			else if (c != '\r') // Skip carriage return
			{
				current += c;
			}
		}

		// Add the last line
		if (!current.isEmpty() || !result.empty())
		{
			result.push_back(current);
		}

		// If string was empty, return at least one empty string
		if (result.empty())
		{
			result.push_back("");
		}

		return result;
	}
}