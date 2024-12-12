#pragma once
#include <Siv3D/IReader.hpp>
#include <Siv3D/TextReader.hpp>
#include <Siv3D/DirectoryWatcher.hpp>
#include <Siv3D/Stopwatch.hpp>
#include "Box.hpp"
#include "Label.hpp"
#include "Util/StyleValueHelper.hpp"
#include "UIState.hpp"

namespace tinyxml2
{
	class XMLDocument;
	class XMLElement;
}

namespace FlexLayout
{
	using EnableHotReload = s3d::YesNo<struct EnableHotReload_tag>;

	class Layout
	{
	public:

		/// @brief XML読み込み直後に呼び出されるコールバック関数
		using OnLoadCallback = std::function<void(Layout&, Box&)>;

		explicit Layout(OnLoadCallback onLoad = nullptr);

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

		/// @brief XMLファイルをファイルパスから読み込む
		/// @param path XMLファイルのパス
		/// @param enableHotReload ホットリロードを有効にするか
		/// @return 成功した場合はtrue、失敗した場合はfalse
		bool load(s3d::FilePathView path, EnableHotReload enableHotReload = EnableHotReload::No);

		/// @brief XMLファイルをファイルパスから読み込む
		/// @param path XMLファイルのパス
		/// @param enableHotReload ホットリロードを有効にするか
		/// @return 成功した場合はtrue、失敗した場合はfalse
		inline bool load(const char32_t* path, EnableHotReload enableHotReload = EnableHotReload::No)
		{
			return load(s3d::FilePath{ path }, enableHotReload);
		}

		/// @brief XMLファイルをファイルパスから読み込む
		/// @param path XMLファイルのパス
		/// @param enableHotReload ホットリロードを有効にするか
		/// @return 成功した場合はtrue、失敗した場合はfalse
		inline bool load(const s3d::FilePath& path, EnableHotReload enableHotReload = EnableHotReload::No)
		{
			return load(path, enableHotReload);
		}

		/// @brief XMLを読み込む
		/// @return 成功した場合はtrue、失敗した場合はfalse
		bool load(s3d::Arg::code_<s3d::String> code);

		/// @brief XMLファイルを読み込む
		/// @return 成功した場合はtrue、失敗した場合はfalse
		template <class Reader, std::enable_if_t<std::is_base_of_v<s3d::IReader, Reader> && !std::is_lvalue_reference_v<Reader>>* = nullptr>
		inline bool load(Reader&& reader)
		{
			load(std::make_unique<Reader>(std::forward<Reader>(reader)));
		}

		/// @brief XMLファイルを読み込む
		/// @return 成功した場合はtrue、失敗した場合はfalse
		inline bool load(std::unique_ptr<s3d::IReader>&& reader)
		{
			load(s3d::Arg::code = s3d::TextReader{ std::move(reader) }.readAll());
		}

		/// @brief XMLを読み込む
		/// @return 成功した場合はtrue、失敗した場合はfalse
		bool load(const tinyxml2::XMLDocument& element);

		/// @brief XMLファイルを再読み込みする
		/// @remark ファイルパス以外からXMLデータを読み込んだ場合は常に失敗します
		/// @return 成功した場合はtrue、失敗した場合はfalse
		bool reload();

		/// @brief ファイルのフルパスを取得する
		const s3d::FilePathView fileFullPath() const;

		/// @brief ホットリロードが有効か
		bool isHotReloadEnabled() const;

		/// @brief ホットリロードの処理を行う
		/// @return 再読み込みが行われた場合はtrue,それ以外はfalse
		bool handleHotReload();

		/// @brief レイアウト制約を設定
		/// @param offset 座標の基準位置
		/// @param width 横幅の限界
		/// @param height 縦幅の限界
		void setConstraints(
			s3d::Vec2 offset = s3d::Vec2::Zero(),
			s3d::Optional<double> width = s3d::none,
			s3d::Optional<double> height = s3d::none
		);

		/// @brief レイアウト制約を設定
		inline void setConstraints(s3d::SizeF size)
		{
			setConstraints(
				s3d::Vec2::Zero(),
				size.x,
				size.y
			);
		}

		/// @brief レイアウト制約を設定
		inline void setConstraints(s3d::RectF rect)
		{
			setConstraints(
				rect.pos,
				rect.w,
				rect.h
			);
		}

		/// @brief レイアウト制約を設定
		inline void setConstraints(s3d::Vec2 offset, s3d::SizeF size)
		{
			setConstraints(
				offset,
				size.x,
				size.y
			);
		}

		/// @brief レイアウトを再計算する
		void calculateLayout();

		template <class State>
		void registerCustomComponent(const s3d::String& tagName, UIStateFactory<State> factory)
		{
			registerCustomComponentImpl(
				tagName,
				[]() -> std::unique_ptr<UIState>
				{ return factory(); }
			);
		}

		/// @brief ルート要素を取得する
		s3d::Optional<Box> document();

		/// @brief ルート要素を設定する
		/// @remark XMLの読込の際に上書きされる可能性があります
		void setDocument(Box root);

		void update();

		void draw() const;

	private:

		struct Impl;

		std::unique_ptr<Impl> m_impl;

		void registerCustomComponentImpl(const s3d::String& tagName, std::unique_ptr<UIState>(*factory)());

	public:

		~Layout();
	};
}
