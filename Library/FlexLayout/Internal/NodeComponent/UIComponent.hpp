#pragma once
#include "../../UIState.hpp"

using namespace s3d;

namespace FlexLayout::Internal
{
	class FlexBoxNode;
}

namespace FlexLayout::Internal::Component
{
	class UIComponent
	{
	public:

		UIComponent(FlexBoxNode& node);

		void copy(const UIComponent& source);

		UIState* state() { return m_state.get(); }

		const UIState* state() const { return m_state.get(); }

		void setState(std::unique_ptr<UIState>&& state);

		void removeState();

		void setAdditionalProperty(const StringView key, const StringView value);

		void unsetAdditionalProperty(const StringView key);

	private:

		FlexBoxNode& m_node;

		std::unique_ptr<UIState> m_state;
	};
}
