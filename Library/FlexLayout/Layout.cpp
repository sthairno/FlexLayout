#include "Layout.hpp"
#include <Siv3D/FileSystem.hpp>
#include "Internal/FlexBoxImpl.hpp"
#include "Internal/XMLLoader.hpp"
#include "Internal/TreeContext.hpp"
#include "Internal/BoxAccessor.hpp"

namespace FlexLayout
{
	struct Layout::Impl
	{
		Layout* intf;

		Layout::OnLoadCallback onLoad;

		s3d::FilePath fileFullPath{ };

		std::unique_ptr<s3d::DirectoryWatcher> dirWatcher{ };

		bool pendingReload = false;

		s3d::Stopwatch reloadTimer;

		std::shared_ptr<Internal::FlexBoxImpl> root;

		bool loadDocument(const tinyxml2::XMLDocument& document)
		{
			if (Internal::XMLLoader{ }.load(root, document))
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

		bool reload()
		{
			pendingReload = false;
			if (fileFullPath.isEmpty())
			{
				return false;
			}

			return loadFile(fileFullPath, dirWatcher.get());
		}

		void update(Optional<float> width, Optional<float> height, Vec2 offset)
		{
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
				reload();
			}

			if (root)
			{
				// スタイルを適用
				Internal::FlexBoxImpl::ApplyStyles(*root);

				// レイアウト計算
				Internal::FlexBoxImpl::CalculateLayout(*root, width, height);

				// グローバル座標の更新
				root->setLayoutOffsetRecursive(offset);
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
		return m_impl->reload();
	}

	bool Layout::isHotReloadEnabled() const
	{
		return m_impl->dirWatcher && m_impl->dirWatcher->isOpen();
	}

	void Layout::update(Optional<float> width, Optional<float> height, Vec2 offset)
	{
		m_impl->update(width, height, offset);
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
		m_impl->root = Internal::BoxAccessor::GetImpl(root);
	}

	Layout::~Layout()
	{ }
}
