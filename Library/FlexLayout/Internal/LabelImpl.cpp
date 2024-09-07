#include "LabelImpl.hpp"

namespace FlexLayout::Internal
{
	struct LabelImpl::Impl
	{
		static YGSize MeasureLabelCallback(
			YGNodeConstRef node,
			float width,
			YGMeasureMode widthMode,
			float height,
			YGMeasureMode heightMode)
		{
			auto& impl = *reinterpret_cast<LabelImpl*>(YGNodeGetContext(node));

			impl.updateConstraints(widthMode != YGMeasureModeUndefined ? width : Math::Inf);
			auto size = impl.computeBoundingBox();

			YGSize result {
				static_cast<float>(size.x),
				static_cast<float>(size.y)
			};

			switch (widthMode)
			{
			case YGMeasureModeUndefined:
				break;
			case YGMeasureModeAtMost:
				result.width = Min(result.width, width);
				break;
			case YGMeasureModeExactly:
				result.width = width;
				break;
			}

			switch (heightMode)
			{
			case YGMeasureModeUndefined:
				break;
			case YGMeasureModeAtMost:
				result.height = Min(result.height, height);
				break;
			case YGMeasureModeExactly:
				result.height = height;
				break;
			}

			return result;
		}

		static float BaselineLabelCallback(YGNodeConstRef node, float, float)
		{
			auto& impl = *reinterpret_cast<LabelImpl*>(YGNodeGetContext(node));
			return static_cast<float>(impl.computeBaseline());
		}
	};
	

	LabelImpl::LabelImpl(std::shared_ptr<TreeContext> context, const StringView tagName)
		: FlexBoxImpl{ context, tagName }
	{
		YGNodeSetMeasureFunc(yogaNode(), Impl::MeasureLabelCallback);
		YGNodeSetBaselineFunc(yogaNode(), Impl::BaselineLabelCallback);
		YGNodeSetNodeType(yogaNode(), YGNodeTypeText);
	}

	void LabelImpl::setText(const StringView text)
	{
		m_text = text;
		YGNodeMarkDirty(yogaNode());
	}

	void LabelImpl::draw(const TextStyle& textStyle, const ColorF& color)
	{
		auto rect = contentAreaRect();

		if (not rect.has_value())
		{
			return;
		}

		auto& style = computedTextStyle();
		auto scale = style.fontRenderingScale();

		// 描画位置を計算
		bool ltr = YGNodeLayoutGetDirection(yogaNode()) != YGDirectionRTL;
		double xAlign = 0.0;
		switch (style.textAlign)
		{
		case TextAlign::Start: xAlign = ltr ? 0.0 : 1.0; break;
		case TextAlign::End: xAlign = ltr ? 1.0 : 0.0; break;
		case TextAlign::Left: xAlign = 0.0; break;
		case TextAlign::Right: xAlign = 1.0; break;
		case TextAlign::Center: xAlign = 0.5; break;
		}

		// テキストを描画
		{
			// シェーダー関連
			HasColor fontHasColor{ style.font.hasColor() };
			if (textStyle.type != TextStyle::Type::Default && (not fontHasColor))
			{
				if (style.font.method() == FontMethod::SDF)
				{
					Graphics2D::SetSDFParameters(textStyle);
				}
				else
				{
					Graphics2D::SetMSDFParameters(textStyle);
				}
			}
			Optional<ScopedCustomShader2D> shader = textStyle.type != TextStyle::Type::CustomShader
				? MakeOptional<ScopedCustomShader2D>(Font::GetPixelShader(style.font.method(), textStyle.type, fontHasColor))
				: none;

			// 描画ロジック
			size_t beginIdx = 0;
			auto nextLineBreakItr = m_lineBreaks.cbegin();
			for (auto [lineIdx, lineWidth] : Indexed(m_lineWidths))
			{
				const size_t endIdx = nextLineBreakItr != m_lineBreaks.cend() ? *nextLineBreakItr++ : m_glyphs.size();

				Vec2 penPos = rect->pos;
				penPos.x += (rect->w - lineWidth) * xAlign;
				penPos.y += computeBaseline(lineIdx);

				for (size_t glyphIdx : Iota(beginIdx, endIdx))
				{
					const auto& glyph = m_glyphs[glyphIdx];

					if (not IsControl(glyph.codePoint))
					{
						const Vec2 glyphPos = penPos + glyph.getBase(scale);
						glyph.texture.scaled(scale, scale).draw(glyphPos, color);
						penPos.x += glyph.xAdvance * scale;
					}
				}

				beginIdx = endIdx;
			}
		}
	}

	void LabelImpl::updateConstraints(double width)
	{
		auto& style = computedTextStyle();
		auto scale = style.fontRenderingScale();

		m_glyphs.clear();
		m_lineBreaks.clear();
		m_lineWidths.clear();
		if (m_text.isEmpty())
		{
			return;
		}

		m_glyphs = style.font.getGlyphs(m_text);
		m_lineWidths.push_back(0.0);

		for (auto [i, glyph] : Indexed(m_glyphs))
		{
			auto& lineWidth = m_lineWidths.back();

			if (glyph.codePoint == U'\n')
			{
				m_lineBreaks.push_back(i + 1);
				m_lineWidths.push_back(0.0);
				continue;
			}

			double xadvance = IsControl(glyph.codePoint) ? 0.0 : glyph.xAdvance * scale;

			if (lineWidth > 0.0 && lineWidth + xadvance > width)
			{
				m_lineBreaks.push_back(i);
				m_lineWidths.push_back(xadvance);
				continue;
			}

			lineWidth += xadvance;
		}
	}

	double LabelImpl::computeBaseline(size_t lineIdx) const
	{
		auto& style = computedTextStyle();

		lineIdx = std::min(lineIdx, lineCount() - 1);

		auto lineHeight = style.lineHeightPx();
		return lineHeight * lineIdx
			+ style.font.ascender() * style.fontRenderingScale()
			+ (lineHeight - style.font.height() * style.fontRenderingScale()) * 0.5;
	}

	SizeF LabelImpl::computeBoundingBox() const
	{
		auto& style = computedTextStyle();
		return {
			m_lineWidths.empty() ? 0.0 : *std::max_element(m_lineWidths.begin(), m_lineWidths.end()),
			style.lineHeightPx() * lineCount()
		};
	}
}
