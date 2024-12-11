#pragma once
#include <Siv3D/Font.hpp>
#include <Siv3D/String.hpp>
#include "../Style/StyleProperty.hpp"
#include "../Style/ComputedTextStyle.hpp"

using namespace s3d;

namespace FlexLayout::Internal
{
	namespace Context
	{
		class StyleContext;
	}
	class FlexBoxNode;
}

namespace FlexLayout::Internal::Component
{
	class StyleComponent
	{
	public:

		StyleComponent(FlexBoxNode& node);

	public:

		void copy(const StyleComponent& source);

		ComputedTextStyle& computedTextStyle() { return m_computedTextStyle; }

		const ComputedTextStyle& computedTextStyle() const { return m_computedTextStyle; }

		void setInlineCssText(const StringView cssText);

		String getInlineCssText() const;

		Array<Style::StyleValue> getStyle(StylePropertyGroup group, const StringView styleName) const;

		bool setStyle(StylePropertyGroup group, const StringView styleName, std::span<const Style::StyleValue> values);

		bool setStyle(StylePropertyGroup group, const StringView styleName, std::span<const Style::ValueInputVariant> values);

		bool removeStyle(StylePropertyGroup group, const StringView styleName);

		void clearStyles(Optional<StylePropertyGroup> group);

		void copyStyles(StylePropertyGroup group, const StyleComponent& source);

		void copyStyles(const StyleComponent& source);

		bool isStyleApplicationScheduled() const { return m_isStyleApplicationScheduled; }

		/// @brief `applyStyleRecursive()`の呼び出しを予約する
		/// @remark 親要素で予約されていた場合、予約をスキップします
		void scheduleStyleApplication();

		Font font() const { return m_font.font; }

		Optional<String> fontId() const { return m_font.id ? none : MakeOptional(m_font.id); }

		void setFont(const Font& font, const StringView fontId = U"");

		void setFont(const StringView fontId);

		void copyFont(const StyleComponent& source);

	private:

		friend class Context::StyleContext;

		FlexBoxNode& m_node;

		struct _FontProperty
		{
			Font font{ };

			String id = U"";
		};

		StylePropertyTable m_styles;

		_FontProperty m_font;

		ComputedTextStyle m_computedTextStyle;

		bool m_isStyleApplicationScheduled = false;

		void applyStylesImpl();
	};
}
