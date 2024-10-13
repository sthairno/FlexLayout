#include "FlexBoxImpl.hpp"

// FlexBoxImplのプロパティに関する実装

namespace FlexLayout::Internal
{
	Optional<String> FlexBoxImpl::getProperty(const StringView key) const
	{
		if (key == U"id")
		{
			return m_id;
		}
		else if (key == U"class")
		{
			return m_classes.join(U" ", U"", U"");
		}
		else if (key == U"style")
		{
			return getInlineCssText();
		}
		else if (key == U"siv3d-font")
		{
			return fontId();
		}
		else if (const auto it = m_additonalProperties.find(key);
			it != m_additonalProperties.end())
		{
			return it->second;
		}

		return none;
	}

	void FlexBoxImpl::setProperty(const StringView key, const StringView value)
	{
		if (key == U"id")
		{
			m_id = value;
		}
		else if (key == U"class")
		{
			m_classes.clear();
			for (auto& className : String{ value }.split(U' '))
			{
				className = className.trimmed();
				if (className.isEmpty())
				{
					continue;
				}
				if (not m_classes.contains(className))
				{
					m_classes.emplace_back(std::move(className));
				}
			}
		}
		else if (key == U"style")
		{
			setInlineCssText(value);
		}
		else if (key == U"siv3d-font")
		{
			setFont(value);
		}
		else
		{
			m_additonalProperties[key] = value;
		}
	}

	bool FlexBoxImpl::removeProperty(const StringView key)
	{
		if (key == U"id")
		{
			m_id.reset();
		}
		else if (key == U"class")
		{
			m_classes.clear();
		}
		else if (key == U"style")
		{
			clearStyles(StylePropertyGroup::Inline);
		}
		else if (key == U"siv3d-font")
		{
			setFont({ }, U"");
		}

		return m_additonalProperties.erase(key);
	}

	bool FlexBoxImpl::hasProperties() const
	{
		return m_id.has_value() ||
			not m_classes.empty() ||
			not m_additonalProperties.empty() ||
			m_styles.group(StylePropertyGroup::Inline) ||
			m_font.font;
	}

	void FlexBoxImpl::clearProperties()
	{
		m_id.reset();
		m_classes.clear();
		clearStyles(StylePropertyGroup::Inline);
		setFont({ }, U"");
		m_additonalProperties.clear();
	}

	void FlexBoxImpl::copyProperties(const FlexBoxImpl& source, bool includeStyles, bool includeFont)
	{
		m_id = source.m_id;
		m_classes = source.m_classes;
		m_additonalProperties = source.m_additonalProperties;

		if (includeStyles)
		{
			copyStyles(StylePropertyGroup::Inline, source);
		}

		if (includeFont)
		{
			copyFont(source);
		}
	}

	bool FlexBoxImpl::addClass(const StringView className)
	{
		String str{ className };
		str.trim();

		if (str.isEmpty())
		{
			return false;
		}
		if (str.contains(U' '))
		{
			throw Error{ U"FlexBox: Class name should not contain space" };
		}

		if (m_classes.contains(str))
		{
			return false;
		}
		else
		{
			m_classes.emplace_back(std::move(str));
			return true;
		}
	}

	bool FlexBoxImpl::removeClass(const StringView className)
	{
		String str{ className };
		str.trim();

		if (str.isEmpty())
		{
			return false;
		}
		if (str.contains(U' '))
		{
			throw Error{ U"FlexBox: Class name should not contain space" };
		}

		auto prevSize = m_classes.size();
		m_classes.remove(str);
		return m_classes.size() != prevSize;
	}
}
