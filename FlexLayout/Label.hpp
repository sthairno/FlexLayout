#pragma once
#include "Common.hpp"
#include "Box.hpp"

namespace FlexLayout
{
	namespace Internal
	{
		class LabelImpl;
	}

	class Label : public Box
	{
	public:

		using Box::Box;

	public:

		/// @brief 描画する文字列を取得する
		/// @remark レイアウトを更新するまでは描画に反映されません
		String text() const;

		/// @brief 描画する文字列を設定する
		void setText(const StringView text);

		/// @brief 文字列を描画する
		void draw(const ColorF& color = Palette::White) const;

		/// @brief 文字列を描画する
		void draw(const TextStyle& textStyle, const ColorF& color = Palette::White) const;

	private:

		Internal::LabelImpl& impl() const;
	};
}
