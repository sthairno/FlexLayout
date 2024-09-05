#pragma once
#include "Common.hpp"
#include "../Style/StyleValue.hpp"
#include "../Util/Thickness.hpp"
#include "ComputedTextStyle.hpp"
#include "StylePropertyDefinition.hpp"

namespace FlexLayout::Internal
{
	class TreeContext;

	class FlexBoxImpl : public std::enable_shared_from_this<FlexBoxImpl>
	{
	public:

		FlexBoxImpl(std::shared_ptr<TreeContext> context, const StringView tagName);

	public:

		std::shared_ptr<TreeContext> context() const { return m_context; }

		YGNodeRef yogaNode() { return m_node; }

		YGNodeConstRef yogaNode() const { return m_node; }

		const StringView tagName() const { return m_tagName; }

		// --ツリー関連(FlexBoxImpl.cpp)--

		FlexBoxImpl* parent() const { return m_parent; }

		const Array<std::shared_ptr<FlexBoxImpl>> children() const { return m_children; }

		void setChildren(Array<std::shared_ptr<FlexBoxImpl>>& children);

		void removeChildren();

		/// @brief ルート要素からの深さを取得する
		/// @return ルート要素の場合は0、それ以外は1以上
		size_t getDepth() const;

		/// @brief ルート要素を取得する
		/// @remark 親要素が存在しない場合、自身を返します
		FlexBoxImpl& getRoot();

		// --プロパティ(FlexBoxPropertyImpl.cpp)--

		Optional<String> getProperty(const StringView key) const;

		void setProperty(const StringView key, const StringView value);

		void removeProperty(const StringView key);

		void clearProperties();

		Optional<String> id() const { return m_id; }

		const Array<String>& classes() const { return m_classes; }

		bool addClass(const StringView className);

		bool removeClass(const StringView className);

		// --スタイル(FlexBoxStyleImpl.cpp)--

		static void ApplyStyles(TreeContext& context);

		ComputedTextStyle& computedTextStyle() { return m_computedTextStyle; }

		const ComputedTextStyle& computedTextStyle() const { return m_computedTextStyle; }

		void setCssText(const StringView cssText);

		String getCssText() const;

		const Array<Style::StyleValue>& getStyle(const StringView styleName);

		Array<Style::StyleValue> getStyle(const StringView styleName) const;

		bool setStyle(const StringView styleName, std::span<const Style::StyleValue> values);

		bool setStyle(const StringView styleName, std::span<const std::variant<std::int32_t, float, const StringView>> values);

		bool removeStyle(const StringView styleName);

		void clearStyles();

		/// @brief `applyStyleRecursive()`の呼び出しを予約する
		/// @remark 親要素で予約されていた場合、予約をスキップします
		void scheduleStyleApplication();

		// --検索(FlexBoxLookupImpl.cpp)--

		void lookupNodesByClassName(Array<std::shared_ptr<FlexBoxImpl>>& list, const String& className, size_t limit = Largest<size_t>);

		std::shared_ptr<FlexBoxImpl> lookupNodeById(const StringView id);

		// --レイアウト(FlexBoxLayoutImpl.cpp)--

		static void CalculateLayout(FlexBoxImpl& node, Optional<float> width, Optional<float> height);

		Optional<Vec2> layoutOffset() const { return m_layoutOffset; }

		void setLayoutOffsetRecursive(Optional<Vec2> offset, bool force = false);

		void resetLayoutOffset();

		Thickness margin() const;

		Thickness border() const;

		Thickness padding() const;

		RectF localMarginAreaRect() const;

		inline RectF localBorderAreaRect() const
		{
			return margin().shrinkRect(localMarginAreaRect());
		}

		inline RectF localPaddingAreaRect() const
		{
			return border().shrinkRect(localBorderAreaRect());
		}

		inline RectF localContentAreaRect() const
		{
			return padding().shrinkRect(localPaddingAreaRect());
		}

		inline Optional<RectF> marginAreaRect() const
		{
			return m_layoutOffset
				? localMarginAreaRect().movedBy(*m_layoutOffset)
				: Optional<RectF>{};
		}

		inline Optional<RectF> borderAreaRect() const
		{
			return m_layoutOffset
				? localBorderAreaRect().movedBy(*m_layoutOffset)
				: Optional<RectF>{};
		}

		inline Optional<RectF> paddingAreaRect() const
		{
			return m_layoutOffset
				? localPaddingAreaRect().movedBy(*m_layoutOffset)
				: Optional<RectF>{};
		}

		inline Optional<RectF> contentAreaRect() const
		{
			return m_layoutOffset
				? localContentAreaRect().movedBy(*m_layoutOffset)
				: Optional<RectF>{};
		}

		// --その他--

		virtual bool isLabel() const noexcept { return false; }

	private:

		// ノード情報

		YGNodeRef m_node;

		const String m_tagName;

		std::shared_ptr<TreeContext> m_context;

		// ツリー関連

		FlexBoxImpl* m_parent = nullptr;

		Array<std::shared_ptr<FlexBoxImpl>> m_children;

		// プロパティ

		Optional<String> m_id;

		Array<String> m_classes;

		HashTable<String, String> m_additonalProperties;

		// スタイル

		struct _StyleProperty
		{
			enum class Event
			{
				None,
				Added,
				Modified,
				Removed,
			};

			const StylePropertyDefinition& definition;

			Array<Style::StyleValue> value;

			bool removed = true;
		};

		using _StylePropertyTable = HashTable<String, _StyleProperty, decltype(StyleProperties)::hasher>;

		_StylePropertyTable m_styles;

		ComputedTextStyle m_computedTextStyle;

		bool m_isStyleApplicationScheduled = false;

		_StyleProperty& getStyleProperty(const StringView styleName);

		_StyleProperty* tryGetStyleProperty(const StringView styleName);

		void applyStylesImpl();

		// レイアウト

		Optional<Vec2> m_layoutOffset;

	public:

		virtual ~FlexBoxImpl();
	};
}
