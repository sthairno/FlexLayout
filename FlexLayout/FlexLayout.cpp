#include "FlexLayout.hpp"
#include "Internal/FlexBoxImpl.hpp"
#include "Internal/XMLLoader.hpp"
#include "Internal/TreeContext.hpp"

namespace FlexLayout
{
	FlexLayout::FlexLayout()
		: m_context(std::make_shared<Internal::TreeContext>())
		, m_loader(std::make_unique<Internal::XMLLoader>(m_context))
	{ }

	bool FlexLayout::load(const char32_t* path, EnableHotReload enableHotReload)
	{
		FilePath fullPath = FileSystem::FullPath(path);
		if (fullPath.isEmpty())
		{
			return false;
		}

		// ホットリロードの設定
		if (enableHotReload)
		{
			if (m_fileFullPath != fullPath)
			{
				FilePath watchDirectory = FileSystem::ParentPath(fullPath);
				m_dirWatcher = std::make_unique<DirectoryWatcher>(watchDirectory);
			}
		}
		else
		{
			m_dirWatcher.reset();
		}

		// ファイル内容読み込み
		String fileContent;

		if (not TextReader(fullPath).readAll(fileContent))
		{
			return false;
		}

		m_fileFullPath = fullPath;
		return load(Arg::code = fileContent);
	}

	bool FlexLayout::load(s3d::Arg::code_<s3d::String> code)
	{
		tinyxml2::XMLDocument document(true, tinyxml2::COLLAPSE_WHITESPACE);
		
		if (document.Parse(code->toUTF8().data()) != tinyxml2::XML_SUCCESS)
		{
			return false;
		}

		return load(document);
	}

	bool FlexLayout::load(const tinyxml2::XMLDocument& document)
	{
		return m_loader->load(m_root, document);
	}

	bool FlexLayout::reload()
	{
		if (m_pendingReload)
		{
			m_pendingReload = false;
		}
		if (m_fileFullPath.isEmpty())
		{
			return false;
		}

		return load(m_fileFullPath, EnableHotReload{ isHotReloadEnabled() });
	}

	bool FlexLayout::isHotReloadEnabled() const
	{
		return m_dirWatcher && m_dirWatcher->isOpen();
	}

	void FlexLayout::update(Optional<float> width, Optional<float> height, Vec2 offset)
	{
		// ファイルの更新検知、再読み込み予約
		if (m_dirWatcher)
		{
			for (auto&& [path, action] : m_dirWatcher->retrieveChanges())
			{
				if ((path == m_fileFullPath) &&
					(action == FileAction::Added || action == FileAction::Modified))
				{
					m_pendingReload = true;
					m_reloadTimer.restart();
					break;
				}
			}
			m_dirWatcher->clearChanges();
		}

		// ファイルの更新を反映 (ホットリロード)
		if (m_pendingReload && m_reloadTimer.elapsed() > 0.5s)
		{
			reload();
		}

		// スタイルを適用
		Internal::FlexBoxImpl::ApplyStyles(*m_context);

		if (m_root)
		{
			// レイアウト計算
			YGNodeCalculateLayout(
				m_root->yogaNode(),
				width.value_or(YGUndefined),
				height.value_or(YGUndefined),
				YGDirectionLTR);

			// グローバル座標の更新
			m_root->setLayoutOffsetRecursive(offset);
		}
	}

	Optional<FlexBox> FlexLayout::document()
	{
		if (m_root)
		{
			return FlexBox{ m_root };
		}

		return none;
	}

	FlexLayout::~FlexLayout()
	{ }
}
