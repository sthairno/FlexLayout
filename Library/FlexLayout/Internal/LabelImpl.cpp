#include "LabelImpl.hpp"
#include <Siv3D/ScopedCustomShader2D.hpp>
#include <Siv3D/Indexed.hpp>
#include <Siv3D/Step.hpp>
#include <Siv3D/Char.hpp>

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

		static float BaselineLabelCallback(YGNodeConstRef node, float width, float)
		{
			auto& impl = *reinterpret_cast<LabelImpl*>(YGNodeGetContext(node));

			if (not impl.m_layoutIsValid)
			{
				impl.updateConstraints(width);
			}

			return static_cast<float>(impl.computeBaseline());
		}

		static void SetupYGNode(YGNodeRef node)
		{
			YGNodeSetMeasureFunc(node, Impl::MeasureLabelCallback);
			YGNodeSetBaselineFunc(node, Impl::BaselineLabelCallback);
			YGNodeSetNodeType(node, YGNodeTypeText);
		}
	};
	
	LabelImpl::LabelImpl(const StringView tagName)
		: FlexBoxImpl{ tagName }
	{
		Impl::SetupYGNode(yogaNode());
	}

	std::shared_ptr<FlexBoxImpl> LabelImpl::clone() const
	{
		auto instance = std::make_shared<LabelImpl>(tagName());

		instance->setPropergateOffset(propergateOffset());
		instance->copyProperties(*this, false, true);
		instance->copyStyles(*this);
		instance->setText(m_text);

		return instance;
	}

	void LabelImpl::setText(const StringView text)
	{
		m_text = text;
		if (m_text != text)
		{
			m_layoutIsValid = false;
			YGNodeMarkDirty(yogaNode());
		}
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

		// MeasureFuncを呼び出す必要がない場合(width: 100%など)、updateConstraintsが呼び出されないためここで計算
		// https://github.com/facebook/yoga/issues/959
		if (not m_layoutIsValid)
		{
			updateConstraints(rect->w);
		}

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
			HasColor fontHasColor{ style.font.hasColor() };
			ColorF renderColor = fontHasColor ? Linear::Palette::White : color;

			// シェーダー関連
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
						glyph.texture.scaled(scale, scale).draw(glyphPos, renderColor);
						penPos.x += glyph.xAdvance * scale;
					}
				}

				beginIdx = endIdx;
			}
		}
	}

	void LabelImpl::updateConstraints(double width)
	{
		m_layoutIsValid = true;

		auto& style = computedTextStyle();
		auto scale = style.fontRenderingScale();

		m_glyphs.clear();
		m_lineBreaks.clear();
		m_lineWidths.clear();
		if (m_text.isEmpty())
		{
			return;
		}

		m_glyphs = style.font.getGlyphs(m_text, Ligature::Yes);
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
