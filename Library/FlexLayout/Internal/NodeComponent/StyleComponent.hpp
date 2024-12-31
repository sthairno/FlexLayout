#pragma once
#include <Siv3D/Font.hpp>
#include <Siv3D/String.hpp>
#include <ThirdParty/parallel_hashmap/btree.h>
#include "../Style/StyleProperty.hpp"
#include "../Style/ComputedTextStyle.hpp"

using namespace s3d;

namespace FlexLayout::Internal
{
	namespace Context
	{
		class StyleContext;
	}
	
	namespace detail
	{
		struct InstalledPropertyState
		{
			StylePropertyDefinitionRef definition;

			StyleProperty* propertyRef = nullptr;

			size_t priority;
		};

		struct PropertyApplicationState
		{
			phmap::btree_map<size_t, InstalledPropertyState> propertyStates;

			size_t priorityCounter = 0;
		};
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

		bool propertyApplicationScheduled() const { return m_propertyApplicationScheduled; }

		/// @brief `applyProperties()`の呼び出しを予約する
		void scheduleStyleApplication();

		Font font() const { return m_fontProperty.font; }

		Optional<String> fontId() const { return m_fontProperty.id ? none : MakeOptional(m_fontProperty.id); }

		void setFont(const Font& font, const StringView fontId = U"");

		void setFont(const StringView fontId);

		void copyFont(const StyleComponent& source);

	private:

		friend class Context::StyleContext;

		FlexBoxNode& m_node;

		// プロパティ

		struct _FontProperty
		{
			Font font{ };

			String id = U"";
		};

		StylePropertyTable m_properties;

		_FontProperty m_fontProperty;

		// 計算済みスタイル

		ComputedTextStyle m_computedTextStyle;

		/// @brief プロパティをComputedTextStyleとYogaに反映
		void applyProperties(detail::PropertyApplicationState& state);

		bool m_propertyApplicationScheduled = false;
	};
}
