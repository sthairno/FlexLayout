#pragma once
#include <Siv3D/String.hpp>
#include <Siv3D/Array.hpp>
#include <Siv3D/HashTable.hpp>
#include <Siv3D/Optional.hpp>

using namespace s3d;

namespace FlexLayout::Internal
{
	class FlexBoxNode;
}

namespace FlexLayout::Internal::Component
{
	class XmlAttributeComponent
	{
	public:

		XmlAttributeComponent(FlexBoxNode& node);

	public:

		void copy(const XmlAttributeComponent& source);

		const String& tagName() const { return m_tagName; }

		void setTagName(const StringView tagName) { m_tagName = tagName; }

		Optional<String> id() const { return m_id; }

		void setId(const Optional<String>& id);

		const Array<String>& classes() const { return m_classes; }

		void setClasses(const Array<String>& classes);

		String getClassText() const;

		void clearClasses() { m_classes.clear(); }

		void setClassText(const StringView classText);

		bool addClass(const StringView className);

		bool removeClass(const StringView className);

		bool lookupNodeByInstance(const std::shared_ptr<FlexBoxNode>& node);

		void lookupNodesByClassName(Array<std::shared_ptr<FlexBoxNode>>& list, const String& className, size_t limit = Largest<size_t>);

		std::shared_ptr<FlexBoxNode> lookupNodeById(const StringView id);

	private:

		FlexBoxNode& m_node;

		String m_tagName;

		Optional<String> m_id;

		Array<String> m_classes;
	};
}
