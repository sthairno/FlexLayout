#include "ParseInlineCSS.hpp"

Array<std::pair<String, String>> FlexLayout::Util::ParseInlineCSS(const String& css)
{
	Array<std::pair<String, String>> result;
	for (const auto& prop : css.split(U';'))
	{
		const auto colonPos = prop.indexOf(U':');
		if (colonPos == String::npos)
		{
			continue;
		}
		result.push_back({ prop.substr(0, colonPos).trim(), prop.substr(colonPos + 1).trim() });
	}
	return result;
}
