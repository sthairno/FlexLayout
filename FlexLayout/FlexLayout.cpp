#include "FlexLayout.hpp"
#include "detail/FlexBoxImpl.hpp"

namespace FlexLayout
{
	FlexLayout::FlexLayout()
		: FlexLayout::FlexBox(std::make_shared<detail::FlexBoxImpl>(std::make_shared<detail::TreeContext>()))
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
		return load(document.RootElement());
	}

	bool FlexLayout::load(const tinyxml2::XMLElement* element)
	{
		if (!element || element->Name() != "Layout"sv)
		{
			return false;
		}

		auto child = element->FirstChildElement();
		if (not child)
		{
			return false;
		}

		bool useWebDefaults = false;
		if (element->QueryBoolAttribute("useWebDefaults", &useWebDefaults) == tinyxml2::XML_SUCCESS)
		{
			m_impl->context().setUseWebDefaults(useWebDefaults);
		}

		// 読み込み処理
		m_impl->context().loadBegin();
		m_impl->load(nullptr, child);
		m_impl->context().loadEnd();

		return true;
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

		// レイアウト計算
		YGNodeCalculateLayout(
			m_impl->yogaNode(),
			width.value_or(YGUndefined),
			height.value_or(YGUndefined),
			YGDirectionLTR);

		// グローバル座標の更新
		m_impl->setLayoutOffsetRecursive(offset);
	}

	FlexLayout::~FlexLayout()
	{ }
}
