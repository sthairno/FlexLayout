#pragma once
#include <Siv3D/Font.hpp>
#include "Style/StyleValue.hpp"
#include "Util/Thickness.hpp"

namespace FlexLayout
{
	namespace Internal
	{
		class FlexBoxImpl;
		struct BoxAccessor;
	}
	class Label;

	using StyleValue = Style::StyleValue;

	class Box
	{
	public:

		Box(std::shared_ptr<Internal::FlexBoxImpl> impl);

		Box(Internal::FlexBoxImpl& impl);

	public:

		/// @brief ローカル座標からグローバル座標へのオフセットを取得する
		s3d::Optional<s3d::Vec2> offset() const;

		/// @brief 親要素のオフセットを子要素に伝播させるか
		/// @details falseの場合、子要素のオフセットの基準位置を{ 0, 0 }にします。Transform2Dなどユーザーが座標変換を行う際に無効にします。
		bool propergateOffset() const;

		/// @brief 親要素のオフセットを子要素に伝播させるか
		/// @details falseの場合、子要素のオフセットの基準位置を{ 0, 0 }にします。Transform2Dなどユーザーが座標変換を行う際に無効にします。
		void setPropergateOffset(bool propergate);

		/// @brief マージンの計算幅を取得
		Thickness margin() const;

		/// @brief ボーダーの計算幅を取得
		Thickness border() const;

		/// @brief パディングの計算幅を取得
		Thickness padding() const;

		/// @brief 親要素が基準としたマージン領域(マージンの外側)の矩形を取得する
		s3d::RectF localMarginAreaRect() const;

		/// @brief 親要素が基準としたボーダー領域(マージンの内側,枠の外側)の矩形を取得する
		s3d::RectF localBorderAreaRect() const;

		/// @brief 親要素が基準としたパディング領域(枠の内側,パディングの外側)の矩形を取得する
		s3d::RectF localPaddingAreaRect() const;

		/// @brief 親要素が基準としたコンテンツ領域(パディングの内側)の矩形を取得する
		s3d::RectF localContentAreaRect() const;

		/// @brief 親要素を基準とした矩形を取得する
		inline s3d::RectF localRect() const { return localBorderAreaRect(); }

		/// @brief マージン領域(マージンの外側)の矩形を取得する
		s3d::Optional<s3d::RectF> marginAreaRect() const;

		/// @brief ボーダー領域(マージンの内側,枠の外側)の矩形を取得する
		s3d::Optional<s3d::RectF> borderAreaRect() const;

		/// @brief パディング領域(枠の内側,パディングの外側)の矩形を取得する
		s3d::Optional<s3d::RectF> paddingAreaRect() const;

		/// @brief コンテンツ領域(パディングの内側)の矩形を取得する
		s3d::Optional<s3d::RectF> contentAreaRect() const;

		/// @brief 計算した矩形を取得する
		inline s3d::Optional<s3d::RectF> rect() const { return borderAreaRect(); }

		/// @brief 指定されたスタイルの値を取得する
		/// @return 値が設定されていない場合は空の配列を返す
		s3d::Array<StyleValue> getStyle(const s3d::StringView styleName) const;

		/// @brief 指定されたスタイルの値を設定する
		/// @return 成功した場合はtrue, 失敗した場合はfalse
		bool setStyle(const s3d::StringView styleName, const s3d::Array<StyleValue>& list);

		/// @brief 指定されたスタイルの値を設定する
		/// @return 成功した場合はtrue, 失敗した場合はfalse
		bool setStyle(const s3d::StringView styleName, StyleValue value);

		/// @brief 指定されたスタイルの値を設定する
		/// @return 成功した場合はtrue, 失敗した場合はfalse
		bool setStyle(const s3d::StringView styleName, StyleValue v1, StyleValue v2);

		/// @brief 指定されたスタイルの値を設定する
		/// @return 成功した場合はtrue, 失敗した場合はfalse
		bool setStyle(const s3d::StringView styleName, StyleValue v1, StyleValue v2, StyleValue v3);

		/// @brief 指定されたスタイルの値を設定する
		/// @return 成功した場合はtrue, 失敗した場合はfalse
		bool setStyle(const s3d::StringView styleName, StyleValue v1, StyleValue v2, StyleValue v3, StyleValue v4);

		/// @brief 指定されたスタイルの値を設定する
		/// @return 成功した場合はtrue, 失敗した場合はfalse
		bool setStyle(
			const s3d::StringView styleName,
			Style::ValueInputVariant value
		);

		/// @brief 指定されたスタイルの値を設定する
		/// @return 成功した場合はtrue, 失敗した場合はfalse
		bool setStyle(
			const s3d::StringView styleName,
			Style::ValueInputVariant v1,
			Style::ValueInputVariant v2
		);

		/// @brief 指定されたスタイルの値を設定する
		/// @return 成功した場合はtrue, 失敗した場合はfalse
		bool setStyle(
			const s3d::StringView styleName,
			Style::ValueInputVariant v1,
			Style::ValueInputVariant v2,
			Style::ValueInputVariant v3
		);

		/// @brief 指定されたスタイルの値を設定する
		/// @return 成功した場合はtrue, 失敗した場合はfalse
		bool setStyle(
			const s3d::StringView styleName,
			Style::ValueInputVariant v1,
			Style::ValueInputVariant v2,
			Style::ValueInputVariant v3,
			Style::ValueInputVariant v4
		);

		s3d::Font font() const;

		void setFont(s3d::Font font);

		s3d::Optional<Box> parent() const;

		s3d::Array<Box> children() const;

		void removeChildren();

		// https://developer.mozilla.org/en-US/docs/Web/API/Node/appendChild
		Box appendChild(Box child);

		// https://developer.mozilla.org/en-US/docs/Web/API/Node/cloneNode
		[[nodiscard]]
		Box cloneNode() const;

		// https://developer.mozilla.org/en-US/docs/Web/API/Node/contains
		bool contains(Box child) const;

		// https://developer.mozilla.org/en-US/docs/Web/API/Node/getRootNode
		Box getRootNode();

		// https://developer.mozilla.org/en-US/docs/Web/API/Node/hasChildNodes
		bool hasChildNodes() const;

		// https://developer.mozilla.org/en-US/docs/Web/API/Node/removeChild
		Box removeChild(Box child);

		// https://developer.mozilla.org/en-US/docs/Web/API/Element/replaceChildren
		void replaceChildren(s3d::Array<Box> newChildren);

		template <class... Box>
		void replaceChildren(Box... newChildren)
		{
			replaceChildren({ newChildren... });
		}

		// https://developer.mozilla.org/en-US/docs/Web/API/Element/getAttribute
		s3d::Optional<s3d::String> getAttribute(s3d::StringView name) const;

		// https://developer.mozilla.org/en-US/docs/Web/API/Element/hasAttribute
		bool hasAttribute(s3d::StringView name) const;

		// https://developer.mozilla.org/en-US/docs/Web/API/Element/hasAttributes
		bool hasAttributes() const;

		// https://developer.mozilla.org/en-US/docs/Web/API/Element/removeAttribute
		void removeAttribute(s3d::StringView name);

		s3d::Array<Box> getElementsByClassName(s3d::StringView className) const;

		s3d::Optional<Box> getElementById(s3d::StringView id) const;

		/// @brief Labelのインスタンスに変換する
		s3d::Optional<Label> asLabel() const;

		/// @brief 枠線を描画する
		void drawFrame(const s3d::ColorF& color = s3d::Palette::White) const;

	protected:

		friend Internal::BoxAccessor;

		std::shared_ptr<Internal::FlexBoxImpl> m_impl;

	public:

		virtual ~Box();
	};
}
