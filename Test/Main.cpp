#define NO_S3D_USING

#include <gtest/gtest.h>
#include <Siv3D.hpp>
#include <FlexLayout/Internal/Config.hpp>

SIV3D_SET(s3d::EngineOption::Renderer::Headless)

using namespace s3d;

class Siv3DTestEnvironment : public ::testing::Environment {
public:

	// Override this to define how to set up the environment.
	void SetUp() override
	{
		FlexLayout::Internal::GetConfig();

		FontAsset::Register(U"fontasset", 10);
		FontAsset::Wait(U"fontasset");
	}

	// Override this to define how to tear down the environment.
	void TearDown() override
	{
		FontAsset::UnregisterAll();
	}
};

void Main()
{
	std::cerr.rdbuf(std::cout.rdbuf());
	Console.open();

	auto argc = System::GetArgc();
	::testing::InitGoogleTest(&argc, System::GetArgv());
	::testing::AddGlobalTestEnvironment(new Siv3DTestEnvironment);
	
	RUN_ALL_TESTS();

	// https://stackoverflow.com/a/1452701
	if constexpr (SIV3D_PLATFORM(WINDOWS))
	{
		system("pause");
	}
	else if constexpr (SIV3D_PLATFORM(MACOS) || SIV3D_PLATFORM(LINUX))
	{
		system("read");
	}
}
