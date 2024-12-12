#include "Layout.hpp"
#include <Siv3D/FileSystem.hpp>
#include "Internal/FlexBoxNode.hpp"
#include "Internal/XMLLoader.hpp"
#include "Internal/TreeContext.hpp"
#include "Internal/Accessor.hpp"

#include "Internal/NodeComponent/LayoutComponent.hpp"

namespace FlexLayout
{
	struct Layout::Impl
	{
		Layout* intf;

		Layout::OnLoadCallback onLoad;

		s3d::FilePath fileFullPath{ };

		std::unique_ptr<s3d::DirectoryWatcher> dirWatcher{ };

		bool pendingReload = false;

		s3d::Stopwatch reloadTimer{ };

		std::shared_ptr<Internal::FlexBoxNode> root;

		s3d::Optional<float> width = none;

		s3d::Optional<float> height = none;

		s3d::Vec2 offset = { 0, 0 };

		Internal::XMLLoader loader{ };

		bool loadDocument(const tinyxml2::XMLDocument& document)
		{
			if (loader.load(root, document))
			{
				if (onLoad)
				{
					Box root{ root };
					onLoad(*intf, root);
				}
				return true;
			}
			return false;
		}

		bool loadFileContent(s3d::StringView content)
		{
			auto utf8Content = content.toUTF8();

			tinyxml2::XMLDocument document(true, tinyxml2::COLLAPSE_WHITESPACE);

			if (document.Parse(utf8Content.data()) != tinyxml2::XML_SUCCESS)
			{
				return false;
			}

			return loadDocument(document);
		}

		bool loadFile(s3d::FilePathView path, bool enableHotReload)
		{
			FilePath fullPath = FileSystem::FullPath(path);
			if (fullPath.isEmpty())
			{
				return false;
			}

			// ホットリロードの設定
			if (enableHotReload)
			{
				if (fileFullPath != fullPath)
				{
					FilePath watchDirectory = FileSystem::ParentPath(fullPath);
					dirWatcher = std::make_unique<DirectoryWatcher>(watchDirectory);
				}
			}
			else
			{
				dirWatcher.reset();
			}

			// ファイル内容読み込み
			String fileContent;

			if (not TextReader(fullPath).readAll(fileContent))
			{
				return false;
			}

			fileFullPath = fullPath;

			return loadFileContent(fileContent);
		}

		bool reloadFile()
		{
			pendingReload = false;
			if (fileFullPath.isEmpty())
			{
				return false;
			}

			return loadFile(fileFullPath, dirWatcher.get());
		}

		bool handleHotReload()
		{
			bool reloaded = false;

			// ファイルの更新検知、再読み込み予約
			if (dirWatcher)
			{
				for (auto&& [path, action] : dirWatcher->retrieveChanges())
				{
					if ((path == fileFullPath) &&
						(action == FileAction::Added || action == FileAction::Modified))
					{
						pendingReload = true;
						reloadTimer.restart();
						break;
					}
				}
				dirWatcher->clearChanges();
			}

			// ファイルの更新を反映 (ホットリロード)
			if (pendingReload && reloadTimer.elapsed() > SecondsF{ 0.5 })
			{
				reloaded = reloadFile();
			}

			return reloaded;
		}

		void calculateLayout()
		{
			if (root)
			{
				// Yogaノードへスタイルを適用
				root->context()
					.getContext<Internal::Context::StyleContext>()
					.applyStyles(*root);

				// Yogaのレイアウト計算
				Internal::CalculateLayout(*root, width, height);

				// ローカル座標からグローバル座標の計算
				root->getComponent<Internal::Component::LayoutComponent>()
					.setLayoutOffsetRecursive(offset);
			}
		}

		void updateUI()
		{
			if (root)
			{
				root->context()
					.getContext<Internal::Context::UIContext>()
					.update(*root);
			}
		}

		void drawUI() const
		{
			if (root)
			{
				root->context()
					.getContext<Internal::Context::UIContext>()
					.draw(*root);
			}
		}
	};

	Layout::Layout(OnLoadCallback onLoad)
		: m_impl(new Impl{
			.intf = this,
			.onLoad = onLoad
		})
	{ }

	bool Layout::load(s3d::FilePathView path, EnableHotReload enableHotReload)
	{
		return m_impl->loadFile(path, enableHotReload.getBool());
	}

	bool Layout::load(s3d::Arg::code_<s3d::String> code)
	{
		return m_impl->loadFileContent(code.value());
	}

	bool Layout::load(const tinyxml2::XMLDocument& document)
	{
		return m_impl->loadDocument(document);
	}

	bool Layout::reload()
	{
		return m_impl->reloadFile();
	}

	bool Layout::isHotReloadEnabled() const
	{
		return m_impl->dirWatcher && m_impl->dirWatcher->isOpen();
	}

	bool Layout::handleHotReload()
	{
		return m_impl->handleHotReload();
	}

	void Layout::setConstraints(s3d::Vec2 offset, s3d::Optional<double> width, s3d::Optional<double> height)
	{
		m_impl->offset = offset;
		m_impl->width = width.map([](double d) { return static_cast<float>(d); });
		m_impl->height = height.map([](double d) { return static_cast<float>(d); });
	}

	void Layout::calculateLayout()
	{
		m_impl->calculateLayout();
	}

	Optional<Box> Layout::document()
	{
		if (m_impl->root)
		{
			return Box{ m_impl->root };
		}
		return none;
	}

	void Layout::setDocument(Box root)
	{
		m_impl->root = Internal::Accessor::GetNode(root);
	}

	void Layout::update()
	{
		m_impl->updateUI();
	}

	void Layout::draw() const
	{
		m_impl->drawUI();
	}

	void Layout::registerCustomComponentImpl(
		const s3d::String& tagName,
		std::unique_ptr<UIState>(*factory)())
	{
		m_impl->loader.registerStateFactory(tagName, factory);
	}

	Layout::~Layout()
	{ }
}
