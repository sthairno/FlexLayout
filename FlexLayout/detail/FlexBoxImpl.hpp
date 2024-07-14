﻿#pragma once
#include "Common.hpp"
#include "TreeContext.hpp"
#include "LabelStatus.hpp"
#include "LabelRenderer.hpp"

namespace FlexLayout::detail
{
	class FlexBoxImpl : public std::enable_shared_from_this<FlexBoxImpl>
	{
	public:

		FlexBoxImpl(std::shared_ptr<TreeContext> context);

	public:

		TreeContext& context() const { return *m_context; }

		YGNodeRef yogaNode() { return m_node; }

		YGNodeConstRef yogaNode() const { return m_node; }

		const StringView tagName() const { return m_tagName; }

		void reset();

		// -- XML読み込み--
		// TODO: XML読み込み機能を切り出し

		void load(FlexBoxImpl* parent, const tinyxml2::XMLElement* element);

		void loadChildren(const tinyxml2::XMLElement* element);

		void loadLabel(const tinyxml2::XMLElement* element);

		// --プロパティ--

		const HashTable<String, String>& properties() const { return m_properties; }

		Optional<String> getProperty(const StringView key) const;

		void setProperty(const StringView key, const StringView value);

		void removeProperty(const StringView key);

		void clearProperties();

		Optional<String> id() const { return m_id; }

		const Array<String>& classes() const { return m_classes; }

		bool addClass(const StringView className);

		bool removeClass(const StringView className);

		const HashTable<String, std::variant<String, float>>& style() const { return m_style; }

		bool setStyle(const StringView styleName, const StringView value);

		bool setStyle(const StringView styleName, const float value);

		bool removeStyle(const StringView styleName);

		// --ツリー関連--

		const Array<std::shared_ptr<FlexBoxImpl>> children() const { return m_children; }

		void setChildren(const Array<std::shared_ptr<FlexBoxImpl>>& children);

		void removeChildren();

		// --検索--
		// TODO: 検索機能を切り出し

		void lookupNodesByClassName(Array<std::shared_ptr<FlexBoxImpl>>& list, const String& className, size_t limit = Largest<size_t>);

		std::shared_ptr<FlexBoxImpl> lookupNodeById(const StringView id);

		// --オフセット--

		Optional<Vec2> layoutOffset() const { return m_layoutOffset; }

		void setLayoutOffsetRecursive(Optional<Vec2> offset, bool force = false);

		void resetLayoutOffset();

		// --ラベル--
		// TODO: ラベル機能を切り出し

		const LabelStatus& labelStatus() const { return m_labelStatus; }

		LabelRenderer* labelRenderer() const { return m_labelRenderer.get(); }

	private:

		// ツリー情報

		std::shared_ptr<TreeContext> m_context;

		Array<std::shared_ptr<FlexBoxImpl>> m_children;

		// ノード情報

		YGNodeRef m_node;

		String m_tagName;

		Optional<String> m_id;

		Array<String> m_classes;

		HashTable<String, std::variant<String, float>> m_style;

		HashTable<String, String> m_additonalProperties;

		void resetStyle();

		void loadStyle(const String& css);

	public:

		~FlexBoxImpl();
	};
}
