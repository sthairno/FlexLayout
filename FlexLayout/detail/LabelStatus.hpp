#pragma once
#include "LabelProperty.hpp"

namespace FlexLayout::detail
{
	struct LabelStatus
	{
		static constexpr double DefaultFontSize = 16.0;
		static constexpr double DefaultLineHeight = 1.5;

		static LabelStatus Create(LabelProperty prop, LabelStatus parent)
		{
			LabelStatus m_status;

			m_status.font = prop.font.value_or(parent.font);
			switch (prop.fontScaleType)
			{
			case FontScaleType::Pixel:
				m_status.fontSizePixel = prop.fontScale;
				break;
			case FontScaleType::Percent:
				m_status.fontSizePixel = parent.fontSizePixel * prop.fontScale / 100.0;
				break;
			case FontScaleType::Em:
				m_status.fontSizePixel = parent.fontSizePixel * prop.fontScale;
				break;
			case FontScaleType::Rem:
				m_status.fontSizePixel = DefaultFontSize * prop.fontScale;
				break;
			default:
				m_status.fontSizePixel = parent.fontSizePixel;
				break;
			}
			m_status.lineHeight = prop.lineHeight.value_or(parent.lineHeight);
			m_status.textAlign = prop.textAlign == TextAlign::Inherit ? parent.textAlign : prop.textAlign;

			return m_status;
		}

		Font font = SimpleGUI::GetFont();
		double fontSizePixel = DefaultFontSize;
		double lineHeight = DefaultLineHeight;
		TextAlign textAlign = TextAlign::Left;
	};
}
