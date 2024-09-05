#pragma once
#include "Common.hpp"
#include "Box.hpp"

namespace FlexLayout
{
	namespace Internal
	{
		class LabelImpl;
	}

	class Label : public Box
	{
	public:

		using Box::Box;

	public:

		const StringView text() const;

		void setText(const StringView text);

		void draw(const ColorF& color = Palette::White) const;

		void draw(const TextStyle& textStyle, const ColorF& color = Palette::White) const;

	private:

		Internal::LabelImpl& impl() const;
	};
}
