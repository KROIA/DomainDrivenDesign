#pragma once
#include "DDD_base.h"
#include <vector>
#include "model/Entity.h"
#include "ValidationResult.h"
#include <QRegularExpression>




namespace DDD
{
	class Validator
	{
	public:
		Validator() = default;
		virtual ~Validator() = default;

		virtual ValidationResult validate(std::shared_ptr<Entity> entity) = 0;

		static bool matches(const std::string& text, const std::string& regex)
		{
			QRegularExpression re(QString::fromStdString(regex));
			QRegularExpressionMatch match = re.match(QString::fromStdString(text));
			return match.hasMatch();
		}

		static bool matches(const std::u16string& text, const std::u16string& regex)
		{
			QRegularExpression re(QString::fromUtf16(regex.c_str()));
			QRegularExpressionMatch match = re.match(QString::fromUtf16(text.c_str()));
			return match.hasMatch();
		}
	private:
	};
}