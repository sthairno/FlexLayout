﻿#include "FlexBoxImpl.hpp"

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
			return getCssText();
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
			setCssText(value);
		}
		else
		{
			m_additonalProperties[key] = value;
		}
	}

	void FlexBoxImpl::removeProperty(const StringView key)
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
			clearStyles();
		}
		else
		{
			m_additonalProperties.erase(key);
		}
	}

	void FlexBoxImpl::clearProperties()
	{
		m_id.reset();
		m_classes.clear();
		clearStyles();
		m_additonalProperties.clear();
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
