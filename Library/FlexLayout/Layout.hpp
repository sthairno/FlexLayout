#pragma once
#include <Siv3D/IReader.hpp>
#include <Siv3D/TextReader.hpp>
#include <Siv3D/DirectoryWatcher.hpp>
#include <Siv3D/Stopwatch.hpp>
#include "Box.hpp"
#include "Label.hpp"
#include "Util/StyleValueHelper.hpp"

namespace tinyxml2
{
	class XMLDocument;
	class XMLElement;
}

namespace FlexLayout
{
	namespace Internal
	{
		class XMLLoader;
	}

	using EnableHotReload = s3d::YesNo<struct EnableHotReload_tag>;

	class Layout
	{
	public:

		using OnLoadCallback = std::function<void(Box&)>;

		Layout(OnLoadCallback onLoad = nullptr);

		inline explicit Layout(const char32_t* path, EnableHotReload enableHotReload = EnableHotReload::No, OnLoadCallback onLoad = nullptr)
			: Layout(onLoad)
		{
			load(path, enableHotReload);
		}

		inline explicit Layout(s3d::FilePathView path, EnableHotReload enableHotReload = EnableHotReload::No, OnLoadCallback onLoad = nullptr)
			: Layout(path.data(), enableHotReload, onLoad) {}

		inline explicit Layout(const s3d::FilePath& path, EnableHotReload enableHotReload = EnableHotReload::No, OnLoadCallback onLoad = nullptr)
			: Layout(path.data(), enableHotReload, onLoad) {}

		inline explicit Layout(s3d::Arg::code_<s3d::String> code, OnLoadCallback onLoad = nullptr)
			: Layout(onLoad)
		{
			load(code);
		}

		template <class Reader, std::enable_if_t<std::is_base_of_v<s3d::IReader, Reader> && !std::is_lvalue_reference_v<Reader>>* = nullptr>
		inline explicit Layout(Reader&& reader, OnLoadCallback onLoad = nullptr)
			: Layout(onLoad)
		{
			load(std::make_unique<Reader>(std::forward<Reader>(reader)));
		}

		inline explicit Layout(std::unique_ptr<s3d::IReader>&& reader, OnLoadCallback onLoad = nullptr)
			: Layout(onLoad)
		{
			load(std::forward<std::unique_ptr<s3d::IReader>>(reader));
		}

		inline explicit Layout(const tinyxml2::XMLDocument& document, OnLoadCallback onLoad = nullptr)
			: Layout(onLoad)
		{
			load(document);
		}

	public:

		OnLoadCallback onLoad;

		bool load(const char32_t* path, EnableHotReload enableHotReload = EnableHotReload::No);

		inline bool load(s3d::FilePathView path, EnableHotReload enableHotReload = EnableHotReload::No)
		{
			return load(path.data(), enableHotReload);
		}

		inline bool load(const s3d::FilePath& path, EnableHotReload enableHotReload = EnableHotReload::No)
		{
			return load(path.data(), enableHotReload);
		}

		bool load(s3d::Arg::code_<s3d::String> code);

		template <class Reader, std::enable_if_t<std::is_base_of_v<s3d::IReader, Reader> && !std::is_lvalue_reference_v<Reader>>* = nullptr>
		inline bool load(Reader&& reader)
		{
			load(std::make_unique<Reader>(std::forward<Reader>(reader)));
		}

		inline bool load(std::unique_ptr<s3d::IReader>&& reader)
		{
			load(s3d::Arg::code = s3d::TextReader{ std::move(reader) }.readAll());
		}

		bool load(const tinyxml2::XMLDocument& element);

		bool reload();

		inline const s3d::FilePathView fileFullPath() const { return m_fileFullPath; }

		bool isHotReloadEnabled() const;

		void update(s3d::Optional<float> width, s3d::Optional<float> height, s3d::Vec2 offset = { 0, 0 });

		inline void update()
		{
			update(s3d::none, s3d::none);
		}

		inline void update(s3d::SizeF size)
		{
			update(
				static_cast<float>(size.x),
				static_cast<float>(size.y)
			);
		}

		inline void update(s3d::RectF rect)
		{
			update(
				static_cast<float>(rect.w),
				static_cast<float>(rect.h),
				rect.pos
			);
		}

		s3d::Optional<Box> document();

	private:

		s3d::FilePath m_fileFullPath{ };

		std::unique_ptr<s3d::DirectoryWatcher> m_dirWatcher;

		bool m_pendingReload = false;

		s3d::Stopwatch m_reloadTimer;

		std::shared_ptr<Internal::FlexBoxImpl> m_root;

		std::unique_ptr<Internal::XMLLoader> m_loader;

	public:

		~Layout();
	};
}
