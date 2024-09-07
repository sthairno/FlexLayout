#pragma once
#include "Common.hpp"
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

	using EnableHotReload = s3d::YesNo<struct PreserveWhitespace_tag>;

	class Layout
	{
	public:

		Layout();

		inline explicit Layout(const char32_t* path, EnableHotReload enableHotReload = EnableHotReload::No)
			: Layout()
		{
			load(path, enableHotReload);
		}

		inline explicit Layout(s3d::FilePathView path, EnableHotReload enableHotReload = EnableHotReload::No)
			: Layout(path.data(), enableHotReload) {}

		inline explicit Layout(const s3d::FilePath& path, EnableHotReload enableHotReload = EnableHotReload::No)
			: Layout(path.data(), enableHotReload) {}

		inline explicit Layout(s3d::Arg::code_<s3d::String> code)
			: Layout()
		{
			load(code);
		}

		template <class Reader, std::enable_if_t<std::is_base_of_v<s3d::IReader, Reader> && !std::is_lvalue_reference_v<Reader>>* = nullptr>
		inline explicit Layout(Reader&& reader)
			: Layout()
		{
			load(std::make_unique<Reader>(std::forward<Reader>(reader)));
		}

		inline explicit Layout(std::unique_ptr<s3d::IReader>&& reader)
			: Layout()
		{
			load(std::forward<std::unique_ptr<s3d::IReader>>(reader));
		}

		inline explicit Layout(const tinyxml2::XMLDocument& document)
			: Layout()
		{
			load(document);
		}

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

		std::shared_ptr<Internal::TreeContext> m_context;

		std::shared_ptr<Internal::FlexBoxImpl> m_root;

		std::unique_ptr<Internal::XMLLoader> m_loader;

	public:

		~Layout();
	};
}
