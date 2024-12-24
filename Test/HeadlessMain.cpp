#ifndef FLEXLAYOUT_GRAPHICAL_TEST

#define NO_S3D_USING

#include <gtest/gtest.h>
#include <FlexLayout/Internal/Config.hpp>
#include <Siv3D.hpp>
#include <Siv3D/Windows/Windows.hpp>

SIV3D_SET(s3d::EngineOption::Renderer::Headless)

class Siv3DTestEnvironment : public ::testing::Environment
{
public:

	void SetUp() override
	{
		FlexLayout::Internal::GetConfig();

		s3d::FontAsset::Register(U"fontasset", 10);
		s3d::FontAsset::Wait(U"fontasset");
	}

	void TearDown() override
	{
		s3d::FontAsset::UnregisterAll();
	}
};

void Main()
{
	auto argc = s3d::System::GetArgc();
	::testing::InitGoogleTest(&argc, s3d::System::GetArgv());
	::testing::AddGlobalTestEnvironment(new Siv3DTestEnvironment);

	RUN_ALL_TESTS();
}

#endif
