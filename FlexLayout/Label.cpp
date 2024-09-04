#include "Label.hpp"
#include "Internal/LabelImpl.hpp"

namespace FlexLayout
{
	const StringView Label::text() const
	{
		return impl().text();
	}

	void Label::setText(const StringView text)
	{
		impl().setText(text);
	}

	void Label::draw(const ColorF& color)
	{
		impl().draw(TextStyle::Default(), color);
	}

	void Label::draw(const TextStyle& textStyle, const ColorF& color)
	{
		impl().draw(textStyle, color);
	}

	Internal::LabelImpl& Label::impl() const
	{
		return *static_cast<Internal::LabelImpl*>(m_impl.get());
	}
}
