#include "FlexBoxImpl.hpp"
#include "TreeContext.hpp"

namespace FlexLayout::Internal
{
	void FlexBoxImpl::setCssText(const StringView cssText)
	{
		size_t beginIdx = 0;
		size_t endIdx = 0;
		while (endIdx < cssText.length())
		{
			beginIdx = endIdx;
			endIdx = cssText.indexOf(U';', beginIdx);
			if (endIdx == String::npos)
			{
				endIdx = cssText.length();
			}

			const StringView propertyText = cssText.substr(beginIdx, endIdx - beginIdx);

			const auto colonPos = propertyText.indexOf(U':');
			if (colonPos == String::npos)
			{
				continue;
			}

			String propertyName{ propertyText.substr(0, colonPos) };
			String propertyValue{ propertyText.substr(colonPos + 1) };

			propertyName.trim();
			propertyValue.trim();

			if (propertyName.isEmpty() || propertyValue.isEmpty())
			{
				continue;
			}

			setStyle(propertyName, propertyValue);
		}
	}

	String FlexBoxImpl::getCssText() const
	{
		String tmp;

		for (const auto& [key, value] : m_styles)
		{
			tmp += U"{}: {}"_fmt(key, value.value.join(U" ", U"", U""));
		}

		return tmp;
	}

	const Array<Style::StyleValue>& FlexBoxImpl::getStyle(const StringView styleName) const
	{
		if (auto itr = m_styles.find(styleName); itr == m_styles.cend())
		{
			auto& entry = itr->second;
			return entry.removed ? Array<Style::StyleValue>{ } : entry.value;
		}

		return { };
	}

	bool FlexBoxImpl::setStyle(const StringView styleName, const Array<Style::StyleValue>& values)
	{
		if (styleName.isEmpty())
		{
			return false;
		}

		if (values.isEmpty())
		{
			return removeStyle(styleName);
		}

		auto& entry = m_styles[styleName];

		entry.value = values;
		entry.removed = false;
		entry.modified = true;

		scheduleStyleApplication();

		return true;
	}

	bool FlexBoxImpl::setStyle(const StringView styleName, const StringView value)
	{
		if (styleName.isEmpty())
		{
			return false;
		}

		if (value.isEmpty())
		{
			return removeStyle(styleName);
		}

		// TODO: パース処理
		return false;
	}

	bool FlexBoxImpl::removeStyle(const StringView styleName)
	{
		if (const auto it = m_styles.find(styleName);
			it != m_styles.end())
		{
			auto& entry = it->second;

			entry.value.clear();
			entry.removed = true;
			entry.modified = false;

			return true;
		}

		return false;
	}

	void FlexBoxImpl::clearStyles()
	{
		for (auto& [_, entry] : m_styles)
		{
			entry.value.clear();
			entry.removed = true;
			entry.modified = false;
		}
	}

	void FlexBoxImpl::scheduleStyleApplication()
	{
		if (m_isStyleApplicationScheduled)
		{
			return;
		}

		// 親要素がスケジュール済みであるかを確認する
		for (FlexBoxImpl* item = this; item; item = item->parent())
		{
			if (item->m_isStyleApplicationScheduled)
			{
				return;
			}
		}

		// 待機リストに追加 (深さ順にソートされる)
		m_context->m_styleApplicationWaitlist.insert({
			getDepth(), weak_from_this()
		});
	}

	void FlexBoxImpl::applyStylesRecursive()
	{
		m_isStyleApplicationScheduled = false;

		// スタイルを適用
		for (auto& [name, entry] : m_styles)
		{
			if (entry.removed)
			{
				
			}
			else if (entry.modified)
			{

			}

			entry.removed = false;
			entry.modified = false;
		}

		// 子要素に再帰
		for (const auto& child : m_children)
		{
			child->applyStylesRecursive();
		}
	}
}
