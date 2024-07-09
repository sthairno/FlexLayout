#pragma once
#include <Siv3D.hpp>
#include "FlexBox.hpp"

namespace tinyxml2
{
	class XMLDocument;
	class XMLElement;
}

namespace FlexLayout
{
	using EnableHotReload = s3d::YesNo<struct PreserveWhitespace_tag>;

	class FlexLayout : public FlexBox
	{
	public:

		FlexLayout();

		inline explicit FlexLayout(const char32_t* path, EnableHotReload enableHotReload = EnableHotReload::No)
			: FlexLayout()
		{
			load(path, enableHotReload);
		}

		inline explicit FlexLayout(s3d::FilePathView path, EnableHotReload enableHotReload = EnableHotReload::No)
			: FlexLayout(path.data(), enableHotReload) {}

		inline explicit FlexLayout(const s3d::FilePath& path, EnableHotReload enableHotReload = EnableHotReload::No)
			: FlexLayout(path.data(), enableHotReload) {}

		inline explicit FlexLayout(s3d::Arg::code_<String> code)
			: FlexLayout()
		{
			load(code);
		}

		template <class Reader, std::enable_if_t<std::is_base_of_v<s3d::IReader, Reader> && !std::is_lvalue_reference_v<Reader>>* = nullptr>
		inline explicit FlexLayout(Reader&& reader)
			: FlexLayout()
		{
			load(std::make_unique<Reader>(std::forward<Reader>(reader)));
		}

		inline explicit FlexLayout(std::unique_ptr<s3d::IReader>&& reader)
			: FlexLayout()
		{
			load(std::forward<std::unique_ptr<s3d::IReader>>(reader));
		}

		inline explicit FlexLayout(const tinyxml2::XMLDocument& document)
			: FlexLayout()
		{
			load(document);
		}

		inline explicit FlexLayout(const tinyxml2::XMLElement* element)
			: FlexLayout()
		{
			load(element);
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
			load(Arg::code = s3d::TextReader{ std::move(reader) }.readAll());
		}

		bool load(const tinyxml2::XMLDocument& element);

		bool load(const tinyxml2::XMLElement* element);

		bool reload();

		inline const s3d::FilePathView fileFullPath() const { return m_fileFullPath; }

		bool isHotReloadEnabled() const;

		void update(s3d::Optional<float> width, s3d::Optional<float> height, s3d::Vec2 offset = { 0, 0 });

		inline void update()
		{
			update(none, none);
		}

		inline void update(s3d::SizeF size)
		{
			update(size.x, size.y);
		}

		inline void update(s3d::RectF rect)
		{
			update(rect.w, rect.h, rect.pos);
		}

	private:

		s3d::FilePath m_fileFullPath{ };

		std::unique_ptr<s3d::DirectoryWatcher> m_dirWatcher;

		bool m_pendingReload = false;

		s3d::Stopwatch m_reloadTimer;

	public:

		~FlexLayout();
	};
}
