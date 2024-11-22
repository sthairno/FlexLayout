#include "Label.hpp"
#include "Internal/LabelImpl.hpp"
#include "Internal/BoxAccessor.hpp"

namespace FlexLayout
{
	Label Label::Create(const StringView text)
	{
		auto impl = std::make_shared<Internal::LabelImpl>(U"label");
		impl->setText(text);
		return Label{ impl };
	}

	String Label::text() const
	{
		return String{ Internal::BoxAccessor::GetLabelImpl(*this)->text()};
	}

	void Label::setText(const StringView text)
	{
		Internal::BoxAccessor::GetLabelImpl(*this)->setText(text);
	}

	void Label::draw(const ColorF& color) const
	{
		Internal::BoxAccessor::GetLabelImpl(*this)->draw(TextStyle::Default(), color);
	}

	void Label::draw(const TextStyle& textStyle, const ColorF& color) const
	{
		Internal::BoxAccessor::GetLabelImpl(*this)->draw(textStyle, color);
	}
}
