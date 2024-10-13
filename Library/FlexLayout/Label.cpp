#include "Label.hpp"
#include "Internal/LabelImpl.hpp"

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
		return String{ impl().text() };
	}

	void Label::setText(const StringView text)
	{
		impl().setText(text);
	}

	void Label::draw(const ColorF& color) const
	{
		impl().draw(TextStyle::Default(), color);
	}

	void Label::draw(const TextStyle& textStyle, const ColorF& color) const
	{
		impl().draw(textStyle, color);
	}

	Internal::LabelImpl& Label::impl() const
	{
		return *static_cast<Internal::LabelImpl*>(m_impl.get());
	}
}
