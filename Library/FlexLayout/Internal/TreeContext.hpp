﻿#pragma once
#include <tuple>
#include <memory>
#include "TreeContext/StyleContext.hpp"

namespace FlexLayout::Internal
{
	class FlexBoxNode;

	/// @brief FlexBoxNodeの同一ツリー内で共有されるデータ
	class TreeContext
	{
	public:

		template <class Type>
		Type& getContext()
		{
			return std::get<Context::StyleContext>(m_contexts);
		}

		template <class Type>
		const Type& getContext() const
		{
			return std::get<Context::StyleContext>(m_contexts);
		}

		void onNewNodeJoin(const std::shared_ptr<FlexBoxNode>& node);

	private:

		std::tuple<Context::StyleContext> m_contexts;
	};
}
