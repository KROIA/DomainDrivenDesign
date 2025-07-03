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
	private:
	};
}