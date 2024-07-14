#include "LabelImpl.hpp"

namespace FlexLayout::detail
{
	static YGSize MeasureLabelCallback(
		YGNodeConstRef node,
		float width,
		YGMeasureMode widthMode,
		float height,
		YGMeasureMode heightMode)
	{
		auto& impl = *reinterpret_cast<FlexBoxImpl*>(YGNodeGetContext(node));
		auto renderer = impl.labelRenderer();

		assert(renderer);

		renderer->setConstraints(widthMode != YGMeasureModeUndefined ? width : Inf<double>);
		auto measuredSize = renderer->size();

		YGSize result{
			static_cast<float>(measuredSize.x),
			static_cast<float>(measuredSize.y)
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
		auto& impl = *reinterpret_cast<FlexBoxImpl*>(YGNodeGetContext(node));
		auto renderer = impl.labelRenderer();

		assert(renderer);

		return static_cast<float>(renderer->baseline());
	}

	LabelImpl::LabelImpl(std::shared_ptr<TreeContext> context, const StringView tagName)
		: FlexBoxImpl{ context, tagName }
	{
		YGNodeSetMeasureFunc(yogaNode(), MeasureLabelCallback);
		YGNodeSetBaselineFunc(yogaNode(), BaselineLabelCallback);
	}

	void LabelImpl::setText(const StringView text)
	{
		m_text = text;
		YGNodeMarkDirty(yogaNode());
	}
}
