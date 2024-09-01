﻿#pragma once
#include "Common.hpp"
#include "Style/StyleValue.hpp"

namespace FlexLayout
{
	namespace Internal
	{
		class FlexBoxImpl;
	}

	using StyleValue = Style::StyleValue;

	class FlexBox
	{
	public:

		FlexBox(std::shared_ptr<Internal::FlexBoxImpl> impl)
			: m_impl(std::move(impl)) { }

	public:

		Optional<Vec2> offset() const;

		RectF localRect() const;

		Optional<RectF> marginAreaRect() const;

		Optional<RectF> borderAreaRect() const;

		Optional<RectF> paddingAreaRect() const;

		Optional<RectF> contentAreaRect() const;

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

		void draw(const s3d::ColorF& color = s3d::Palette::White) const;

		s3d::Array<FlexBox> children() const;

		s3d::Array<FlexBox> getElementsByClassName(s3d::StringView className) const;

		s3d::Optional<FlexBox> getElementById(s3d::StringView id) const;

	protected:

		std::shared_ptr<Internal::FlexBoxImpl> m_impl;
	};
}
