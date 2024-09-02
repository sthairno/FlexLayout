#pragma once
#include "Common.hpp"
#include "../Style/StyleValue.hpp"
#include "ComputedTextStyle.hpp"

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

		Optional<Vec2> layoutOffset() const { return m_layoutOffset; }

		void setLayoutOffsetRecursive(Optional<Vec2> offset, bool force = false);

		void resetLayoutOffset();

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

		struct _StyleValueEntry
		{
			enum class Event
			{
				None,
				Added,
				Modified,
				Removed,
			};

			Array<Style::StyleValue> value;

			Event event = Event::None;

			bool removed = true;
		};

		using _StyleValueTable = HashTable<String, _StyleValueEntry>;

		_StyleValueTable m_styles;

		ComputedTextStyle m_computedTextStyle;

		bool m_isStyleApplicationScheduled = false;

		void applyStylesImpl();

		// レイアウト

		Optional<Vec2> m_layoutOffset;

	public:

		virtual ~FlexBoxImpl();
	};
}
