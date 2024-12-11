#pragma once
#include <Siv3D/Font.hpp>
#include "Style/StyleValue.hpp"
#include "Thickness.hpp"

namespace FlexLayout
{
	namespace Internal
	{
		class FlexBoxNode;
		struct Accessor;
	}
	class Label;

	using StyleValue = Style::StyleValue;

	class Box
	{
	public:

		Box(const std::shared_ptr<Internal::FlexBoxNode>& node);

	public:

		// Layout

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

		// Inline Styles

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

		/// @brief 指定されたスタイルの値を削除する
		/// @return 成功した場合はtrue, 失敗した場合はfalse
		bool unsetStyle(const s3d::StringView styleName);

		// Font

		/// @brief フォントの設定値を取得する
		s3d::Font font() const;

		/// @brief 使用するフォントを設定する
		/// @remark 無効なフォントの場合、親要素またはデフォルトのフォント設定を継承します
		void setFont(s3d::Font font);

		/// @brief フォントの設定を解除する
		void unsetFont();

		// Tree

		/// @brief 親ノードを取得する
		/// @return 親ノードが存在しない場合はnone
		s3d::Optional<Box> parent() const;

		/// @brief 子ノードを取得する
		s3d::Array<Box> children() const;

		/// @brief 子ノードをすべて削除する
		void removeChildren();

		/// @brief 子ノードを末尾に追加する
		/// @remark https://developer.mozilla.org/en-US/docs/Web/API/Node/appendChild
		/// @param child 追加する子ノード
		/// @throw FlexLayout::InvalidTreeOperationError 不正なツリーを作成した場合
		/// @return 追加した子ノード、childと同じインスタンスになります
		Box appendChild(Box child);

		/// @brief ノードを複製する
		/// @remark https://developer.mozilla.org/en-US/docs/Web/API/Node/cloneNode
		/// @param deep trueの場合、子ノードも含めて複製します
		/// @return 複製したノード
		[[nodiscard]]
		Box cloneNode(bool deep = false) const;

		/// @brief このノード内に指定されたノードが存在するかどうかを判定する
		/// @remark https://developer.mozilla.org/en-US/docs/Web/API/Node/contains
		/// @param node 検索するノード
		/// @return このノードと同一の場合、または子,孫ノードの場合はtrue
		bool contains(Box node) const;

		/// @brief ルート要素を取得する
		/// @return ツリーのルート要素、親ノードが存在しない場合は自身を返します
		Box getRootNode();

		/// @brief 子ノードを持っているかを判定する
		/// @remark https://developer.mozilla.org/en-US/docs/Web/API/Node/hasChildNodes
		bool hasChildNodes() const;

		/// @brief 子ノードを削除する
		/// @remark https://developer.mozilla.org/en-US/docs/Web/API/Node/removeChild
		/// @param child 削除するノード
		/// @throw FlexLayout::NotFoundError ノードが見つからない場合
		/// @return 削除したノード、childと同じインスタンスになります
		Box removeChild(Box child);

		/// @brief 子ノードをすべて置き換える
		/// @remark https://developer.mozilla.org/en-US/docs/Web/API/Element/replaceChildren
		void replaceChildren(s3d::Array<Box> newChildren);

		/// @brief 子ノードをすべて置き換える
		/// @remark https://developer.mozilla.org/en-US/docs/Web/API/Element/replaceChildren
		template <class... Box>
		void replaceChildren(Box... newChildren)
		{
			replaceChildren({ newChildren... });
		}

		// Attributes

		/// @brief 属性の値を取得する
		/// @remark https://developer.mozilla.org/en-US/docs/Web/API/Element/getAttribute
		/// @param name 属性名
		/// @return 設定値、登録されていない場合はnone
		s3d::Optional<s3d::String> getAttribute(s3d::StringView name) const;

		/// @brief 属性の値を設定する
		/// @remark https://developer.mozilla.org/ja/docs/Web/API/Element/setAttribute
		/// @param name 属性名
		/// @param value 設定値
		void setAttribute(s3d::StringView name, s3d::StringView value);

		/// @brief 属性を削除する
		/// @remark https://developer.mozilla.org/ja/docs/Web/API/Element/removeAttribute
		/// @param name 属性名
		/// @return 削除に成功した場合はtrue
		bool removeAttribute(s3d::StringView name);

		/// @brief 属性が存在するかを判定する
		/// @remark https://developer.mozilla.org/en-US/docs/Web/API/Element/hasAttribute
		/// @param name 属性名
		/// @return 存在する場合はtrue
		bool hasAttribute(s3d::StringView name) const;

		// Query

		s3d::Array<Box> getElementsByClassName(s3d::StringView className) const;

		s3d::Optional<Box> getElementById(s3d::StringView id) const;

		// Others

		/// @brief Labelのインスタンスに変換する
		s3d::Optional<Label> asLabel() const;

		/// @brief 枠線を描画する
		/// @param color 枠線の色
		void drawFrame(const s3d::ColorF& color = s3d::Palette::White) const;

	protected:

		friend Internal::Accessor;

		std::shared_ptr<Internal::FlexBoxNode> m_node;

	public:

		virtual ~Box();
	};
}
