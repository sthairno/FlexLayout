#define NO_S3D_USING

#include <Siv3D.hpp>
#include <FlexLayout.hpp>
#include <yoga/Yoga.h>

SIV3D_SET(s3d::EngineOption::Renderer::Headless)

using namespace FlexLayout::Literals;

void Main()
{
	{
		bool called = false;
		auto callback = [&](FlexLayout::Box& box) -> void
			{
				called = true;
			};
		FlexLayout::Layout layout(callback);

		assert(not called);

		assert(layout.load(s3d::Arg::code = U"<Layout></Layout>"));

		assert(called);
	}
}
