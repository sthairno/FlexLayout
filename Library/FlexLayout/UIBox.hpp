#pragma once
#include "Box.hpp"
#include "UIState.hpp"

namespace FlexLayout
{
	namespace detail
	{
		class UIBoxBase : public Box
		{
		public:

			using Box::Box;

		protected:

			UIState* getState();

			const UIState* getState() const;
		};
	}

	template <class State>
	class UIBox : public detail::UIBoxBase
	{
	public:

		using UIBoxBase::UIBoxBase;

		template <class OtherState, std::enable_if_t<std::is_base_of_v<State, OtherState>>* = nullptr>
		UIBox(const UIBox<OtherState>& other)
			: UIBoxBase(other)
		{ }

		template <class OtherState, std::enable_if_t<std::is_base_of_v<State, OtherState>>* = nullptr>
		UIBox(UIBox<OtherState>&& other)
			: UIBoxBase(std::move(other))
		{ }

	public:

		State& state() { return *reinterpret_cast<State*>(getState()); }

		const State& state() const { return *reinterpret_cast<const State*>(getState()); }

		State* operator->() { return reinterpret_cast<State*>(getState()); }

		const State* operator->() const { return reinterpret_cast<const State*>(getState()); }
	};

	template <class State>
	s3d::Optional<UIBox<State>> Box::as() const
	{
		static_assert(std::is_base_of_v<UIState, State>);
		if (auto state = tryGetUIState())
		{
			if (dynamic_cast<State*>(state))
			{
				return UIBox<State>{ m_node };
			}
		}
		return s3d::none;
	}
}
