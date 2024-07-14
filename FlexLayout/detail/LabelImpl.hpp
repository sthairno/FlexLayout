#pragma once
#include "Common.hpp"
#include "FlexBoxImpl.hpp"

namespace FlexLayout::detail
{
	class LabelImpl : public FlexBoxImpl
	{
	public:

		LabelImpl(std::shared_ptr<TreeContext> context, const StringView tagName);

	public:

		const StringView text() const { return m_text; }

		void setText(const StringView text);

		void draw(TextStyle textStyle, ColorF color);

	private:

		String m_text;
	};
}
