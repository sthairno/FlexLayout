#pragma once
#include "Common.hpp"
#include "Style/StyleValue.hpp"
#include "Util/Thickness.hpp"

namespace FlexLayout
{
	namespace Internal
	{
		class TreeContext;
		class FlexBoxImpl;
	}
	class Label;

	using StyleValue = Style::StyleValue;

	class Box
	{
	public:

		Box(std::shared_ptr<Internal::FlexBoxImpl> impl)
			: m_impl(std::move(impl)) { }

	public:

		Optional<Vec2> offset() const;

		RectF localRect() const;

		Thickness margin() const;

		Thickness border() const;

		Thickness padding() const;

		Optional<RectF> marginAreaRect() const;

		Optional<RectF> borderAreaRect() const;

		Optional<RectF> paddingAreaRect() const;

		Optional<RectF> contentAreaRect() const;

		Array<StyleValue> getStyle(const StringView key) const;

		bool setStyle(const StringView key, const Array<StyleValue>& list);

		bool setStyle(const StringView key, StyleValue value);

		bool setStyle(const StringView key, StyleValue v1, StyleValue v2);

		bool setStyle(const StringView key, StyleValue v1, StyleValue v2, StyleValue v3);

		bool setStyle(const StringView key, StyleValue v1, StyleValue v2, StyleValue v3, StyleValue v4);

		bool setStyle(
			const StringView key,
			Style::ValueInputVariant value
		);

		bool setStyle(
			const StringView key,
			Style::ValueInputVariant v1,
			Style::ValueInputVariant v2
		);

		bool setStyle(
			const StringView key,
			Style::ValueInputVariant v1,
			Style::ValueInputVariant v2,
			Style::ValueInputVariant v3
		);

		bool setStyle(
			const StringView key,
			Style::ValueInputVariant v1,
			Style::ValueInputVariant v2,
			Style::ValueInputVariant v3,
			Style::ValueInputVariant v4
		);

		s3d::Array<Box> children() const;

		s3d::Array<Box> getElementsByClassName(s3d::StringView className) const;

		s3d::Optional<Box> getElementById(s3d::StringView id) const;

		s3d::Optional<Label> asLabel() const;

		void drawFrame(const ColorF& color = Palette::White) const;

	protected:

		std::shared_ptr<Internal::FlexBoxImpl> m_impl;

	public:

		virtual ~Box();
	};
}
