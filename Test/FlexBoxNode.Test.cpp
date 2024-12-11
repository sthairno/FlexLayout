#include <gtest/gtest.h>
#include <Siv3D.hpp>
#include <tinyxml2.h>
#include "FlexLayout/Layout.hpp"
#include "FlexLayout/Internal/FlexBoxImpl.hpp"
#include "FlexLayout/Internal/LabelImpl.hpp"
#include "FlexLayout/Internal/XMLLoader.hpp"
#include "FlexLayout/Error.hpp"

namespace FlexLayout::Internal
{
	TEST(FlexBoxTreeTest, SetChildren)
	{
		auto root = std::make_shared<FlexBoxImpl>(U"dummy");

		auto child1 = std::make_shared<FlexBoxImpl>(U"child1");
		auto child2 = std::make_shared<FlexBoxImpl>(U"child2");

		Array<std::shared_ptr<FlexBoxImpl>> children{
			child1, child2
		};
		ASSERT_NO_THROW(root->setChildren(children));
		ASSERT_EQ(root->children().size(), 2);
		ASSERT_EQ(root->children()[0], child1);
		ASSERT_EQ(root->children()[1], child2);

		ASSERT_EQ(&root->context(), &child1->context());
		ASSERT_EQ(&root->context(), &child2->context());
	}

	TEST(FlexBoxTreeTest, SetChildren_CircularReference)
	{
		auto root = std::make_shared<FlexBoxImpl>(U"dummy");

		auto child1 = std::make_shared<FlexBoxImpl>(U"child1");
		auto child2 = std::make_shared<FlexBoxImpl>(U"child2");

		Array<std::shared_ptr<FlexBoxImpl>> children{
			child1, child2, root
		};
		ASSERT_THROW(root->setChildren(children), InvalidTreeOperationError);

		ASSERT_EQ(root->children().size(), 0);

		ASSERT_NE(&root->context(), &child1->context());
		ASSERT_NE(&root->context(), &child2->context());
	}

	TEST(FlexBoxTreeTest, SetChildren_CircularReference2)
	{
		auto root = std::make_shared<FlexBoxImpl>(U"dummy");

		auto child1 = std::make_shared<FlexBoxImpl>(U"child1");
		auto child2 = std::make_shared<FlexBoxImpl>(U"child2");
		auto child3 = std::make_shared<FlexBoxImpl>(U"child3");

		child2->appendChild(child3);

		Array<std::shared_ptr<FlexBoxImpl>> children{
			child1, child2, child3
		};
		ASSERT_THROW(root->setChildren(children), InvalidTreeOperationError);

		ASSERT_EQ(root->children().size(), 0);

		ASSERT_NE(&root->context(), &child1->context());
		ASSERT_NE(&root->context(), &child2->context());
		ASSERT_NE(&root->context(), &child3->context());
	}

	TEST(FlexBoxTreeTest, SetChildren_DuplicatedChild)
	{
		auto root = std::make_shared<FlexBoxImpl>(U"dummy");

		auto child1 = std::make_shared<FlexBoxImpl>(U"child1");
		auto child2 = std::make_shared<FlexBoxImpl>(U"child2");

		Array<std::shared_ptr<FlexBoxImpl>> children{
			child1, child2, child1
		};
		ASSERT_THROW(root->setChildren(children), InvalidTreeOperationError);

		ASSERT_EQ(root->children().size(), 0);

		ASSERT_NE(&root->context(), &child1->context());
		ASSERT_NE(&root->context(), &child2->context());
	}

	TEST(FlexBoxTreeTest, RemoveChildren)
	{
		auto root = std::make_shared<FlexBoxImpl>(U"dummy");

		auto child1 = std::make_shared<FlexBoxImpl>(U"child1");
		auto child2 = std::make_shared<FlexBoxImpl>(U"child2");

		Array<std::shared_ptr<FlexBoxImpl>> children{
			child1, child2
		};
		root->setChildren(children);

		ASSERT_NO_THROW(root->removeChildren());
		ASSERT_EQ(root->children().size(), 0);
		ASSERT_EQ(child1->parent(), nullptr);
		ASSERT_EQ(child2->parent(), nullptr);

		ASSERT_NE(&root->context(), &child1->context());
		ASSERT_NE(&root->context(), &child2->context());
	}

	TEST(FlexBoxTreeTest, RemoveChildren_WithGrandChildren)
	{
		auto root = std::make_shared<FlexBoxImpl>(U"dummy");

		auto child1 = std::make_shared<FlexBoxImpl>(U"child1");
		auto child2 = std::make_shared<FlexBoxImpl>(U"child2");
		auto child3 = std::make_shared<FlexBoxImpl>(U"child3");

		child2->appendChild(child3);

		Array<std::shared_ptr<FlexBoxImpl>> children{
			child1, child2
		};
		root->setChildren(children);

		ASSERT_NO_THROW(root->removeChildren());
		ASSERT_EQ(root->children().size(), 0);
		ASSERT_EQ(child1->parent(), nullptr);
		ASSERT_EQ(child2->parent(), nullptr);
		ASSERT_EQ(child3->parent(), child2.get());

		ASSERT_NE(&root->context(), &child1->context());
		ASSERT_NE(&root->context(), &child2->context());
		ASSERT_NE(&root->context(), &child3->context());
	}

	TEST(FlexBoxTreeTest, InsertChild)
	{
		auto root = std::make_shared<FlexBoxImpl>(U"dummy");

		auto child1 = std::make_shared<FlexBoxImpl>(U"child1");
		auto child2 = std::make_shared<FlexBoxImpl>(U"child2");

		root->appendChild(child1);
		root->insertChild(child2, 0);

		ASSERT_EQ(root->children().size(), 2);
		ASSERT_EQ(root->children()[0], child2);
		ASSERT_EQ(root->children()[1], child1);

		ASSERT_EQ(child1->parent(), root.get());
		ASSERT_EQ(child2->parent(), root.get());

		ASSERT_EQ(&root->context(), &child1->context());
		ASSERT_EQ(&root->context(), &child2->context());
	}

	TEST(FlexBoxTreeTest, InsertChild_CircularReference)
	{
		auto root = std::make_shared<FlexBoxImpl>(U"dummy");

		auto child1 = std::make_shared<FlexBoxImpl>(U"child1");
		auto child2 = std::make_shared<FlexBoxImpl>(U"child2");

		root->appendChild(child1);
		child1->appendChild(child2);

		ASSERT_THROW(child2->insertChild(root, 0), InvalidTreeOperationError);

		ASSERT_EQ(child2->children().size(), 0);
	}

	TEST(FlexBoxTreeTest, RemoveChild)
	{
		auto root = std::make_shared<FlexBoxImpl>(U"dummy");

		auto child1 = std::make_shared<FlexBoxImpl>(U"child1");
		auto child2 = std::make_shared<FlexBoxImpl>(U"child2");

		root->appendChild(child1);
		root->appendChild(child2);

		ASSERT_NO_THROW(root->removeChild(child1));
		ASSERT_EQ(root->children().size(), 1);
		ASSERT_EQ(root->children()[0], child2);

		ASSERT_EQ(child1->parent(), nullptr);
		ASSERT_EQ(child2->parent(), root.get());

		ASSERT_EQ(&root->context(), &child2->context());
	}

	TEST(FlexBoxTreeTest, RemoveChild_WithGrandChildren)
	{
		auto root = std::make_shared<FlexBoxImpl>(U"dummy");

		auto child1 = std::make_shared<FlexBoxImpl>(U"child1");
		auto child2 = std::make_shared<FlexBoxImpl>(U"child2");
		auto child3 = std::make_shared<FlexBoxImpl>(U"child3");

		child2->appendChild(child3);

		root->appendChild(child1);
		root->appendChild(child2);

		ASSERT_NO_THROW(root->removeChild(child2));
		ASSERT_EQ(root->children().size(), 1);
		ASSERT_EQ(root->children()[0], child1);

		ASSERT_EQ(child1->parent(), root.get());
		ASSERT_EQ(child2->parent(), nullptr);
		ASSERT_EQ(child3->parent(), child2.get());

		ASSERT_EQ(&root->context(), &child1->context());
		ASSERT_NE(&root->context(), &child2->context());
		ASSERT_NE(&root->context(), &child3->context());
	}

	TEST(FlexBoxTreeTest, RemoveChild_ChildNotFound)
	{
		auto root = std::make_shared<FlexBoxImpl>(U"dummy");

		auto child1 = std::make_shared<FlexBoxImpl>(U"child1");

		ASSERT_THROW(root->removeChild(child1), NotFoundError);
	}

	TEST(FlexBoxTreeTest, GetDepth)
	{
		auto root = std::make_shared<FlexBoxImpl>(U"dummy");

		ASSERT_EQ(root->getDepth(), 0);

		auto child1 = std::make_shared<FlexBoxImpl>(U"child1");
		root->appendChild(child1);

		ASSERT_EQ(child1->getDepth(), 1);

		auto child2 = std::make_shared<FlexBoxImpl>(U"child2");
		child1->appendChild(child2);

		ASSERT_EQ(child2->getDepth(), 2);
	}

	TEST(FlexBoxTreeTest, GetRoot)
	{
		auto root = std::make_shared<FlexBoxImpl>(U"dummy");

		ASSERT_EQ(&root->getRoot(), root.get());

		auto child1 = std::make_shared<FlexBoxImpl>(U"child1");
		root->appendChild(child1);

		ASSERT_EQ(&child1->getRoot(), root.get());

		auto child2 = std::make_shared<FlexBoxImpl>(U"child2");
		child1->appendChild(child2);

		ASSERT_EQ(&child2->getRoot(), root.get());
	}

	TEST(FlexBoxTreeTest, Clone)
	{
		std::shared_ptr<FlexBoxImpl> root;

		tinyxml2::XMLDocument document{};
		document.Parse(R"(
			<Layout>
				<Box id="test1" class="abc def" siv3d-font="fontasset" style="width: 100px" hoge="fuga">
					<Label id="test2">foobar</Label>
				</Box>
			</Layout>
		)");

		XMLLoader{}.load(root, document);

		auto clone = root->deepClone();

		ASSERT_EQ(clone->id(), root->id());
		ASSERT_EQ(clone->classes(), root->classes());
		ASSERT_EQ(clone->getProperty(U"hoge"), root->getProperty(U"hoge"));
		ASSERT_EQ(clone->fontId(), root->fontId());
		ASSERT_EQ(clone->font(), root->font());
		ASSERT_EQ(clone->getInlineCssText(), root->getInlineCssText());

		ASSERT_EQ(clone->children().size(), root->children().size());
		ASSERT_EQ(clone->children()[0]->id(), root->children()[0]->id());
		ASSERT_EQ(clone->children()[0]->type(), NodeType::Label);
		ASSERT_EQ(reinterpret_cast<LabelImpl*>(clone->children()[0].get())->text(), U"foobar");

		ASSERT_NE(&clone->context(), &root->context());
	}
}
