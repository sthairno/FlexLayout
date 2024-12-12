#pragma once
#include <yoga/Yoga.h>
#include <Siv3D/Array.hpp>
#include <Siv3D/Optional.hpp>
#include <Siv3D/HashTable.hpp>

using namespace s3d;

namespace FlexLayout::Internal
{
	namespace Component
	{
		class LayoutComponent;
		class StyleComponent;
		class XmlAttributeComponent;
		class TextComponent;
		class UIComponent;
	}
	class TreeContext;

	struct FlexBoxNodeOptions
	{
		bool textNode = false;
		bool uiNode = false;
	};

	class FlexBoxNode : public std::enable_shared_from_this<FlexBoxNode>
	{
	public:

		FlexBoxNode(FlexBoxNodeOptions options = {});

	public:

		// --- コンポーネント関連 ---

		YGNodeRef yogaNode() { return m_yogaNode; }

		YGNodeConstRef yogaNode() const { return m_yogaNode; }

		template <class Type>
		Type& getComponent()
		{
			auto ptr = std::get<std::unique_ptr<Type>>(m_components).get();

			assert(ptr);

			return *ptr;
		}

		template <class Type>
		const Type& getComponent() const
		{
			auto ptr = std::get<std::unique_ptr<Type>>(m_components).get();

			assert(ptr);

			return *ptr;
		}

		// --- ツリー関連 ---

		FlexBoxNode* parent() const { return m_parent; }

		const Array<std::shared_ptr<FlexBoxNode>>& children() const { return m_children; }

		void setChildren(const Array<std::shared_ptr<FlexBoxNode>>& children);

		void removeChildren();

		void insertChild(const std::shared_ptr<FlexBoxNode>& child, size_t index);

		void appendChild(const std::shared_ptr<FlexBoxNode>& child);

		void removeChild(const std::shared_ptr<FlexBoxNode>& child);

		TreeContext& context();

		const TreeContext* context() const { return m_context.get(); }

		/// @brief ルート要素からの深さを取得する
		/// @return ルート要素の場合は0、それ以外は1以上
		size_t getDepth() const;

		/// @brief ルート要素を取得する
		/// @remark 親要素が存在しない場合、自身を返します
		FlexBoxNode& getRoot();

		/// @brief ルート要素を取得する
		/// @remark 親要素が存在しない場合、自身を返します
		const FlexBoxNode& getRoot() const;

		/// @brief この要素のみを複製する
		[[nodiscard]]
		virtual std::shared_ptr<FlexBoxNode> clone() const;

		/// @brief 子要素を含めて複製する
		[[nodiscard]]
		std::shared_ptr<FlexBoxNode> deepClone() const;

		bool isTextNode() const;

		bool isUINode() const;

		static bool BelongsToSameTree(const FlexBoxNode& a, const FlexBoxNode& b);

		// --- プロパティ関連 ---

		Optional<String> getProperty(const StringView key) const;

		void setProperty(const StringView key, const StringView value);

		bool removeProperty(const StringView key);

		void clearProperties();

		const HashTable<String, String>& getAdditionalProperties() const { return m_additonalProperties; }

	private:

		FlexBoxNode* m_parent = nullptr;

		Array<std::shared_ptr<FlexBoxNode>> m_children;

		YGNodeRef m_yogaNode;

		std::tuple<
			std::unique_ptr<Component::LayoutComponent>,
			std::unique_ptr<Component::StyleComponent>,
			std::unique_ptr<Component::XmlAttributeComponent>,
			std::unique_ptr<Component::TextComponent>,
			std::unique_ptr<Component::UIComponent>
		> m_components;

		std::shared_ptr<TreeContext> m_context;

		HashTable<String, String> m_additonalProperties;

		void setContext(const std::shared_ptr<TreeContext>& context);

		void setContextImpl(const std::shared_ptr<TreeContext>& context);

	public:

		~FlexBoxNode();
	};

	void CalculateLayout(FlexBoxNode& node, Optional<float> width, Optional<float> height);
}
