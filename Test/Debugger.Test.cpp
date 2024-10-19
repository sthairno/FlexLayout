#include <gtest/gtest.h>
#include <Siv3D.hpp>
#include <fstream>
#include "FlexLayout/Layout.hpp"
#include "FlexLayout/Debugger.hpp"

namespace FlexLayout
{
	TEST(DebuggerTest, DumpTree)
	{
		Layout layout{ Arg::code = UR"(
			<Layout>
				<Box id="test1" class="abc def">
					<Box id="test2">
						<Box class="abc def"/>
						<Box/>
						<Label/>
					</Box>
					<Box>
						<Box/>
						<Label/>
					</Box>
				</Box>
			</Layout>
		)" };

		// https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
		std::ifstream ifs{ "Debugger_output.txt" };
		std::stringstream buffer;
		buffer << ifs.rdbuf();
		std::string expected = buffer.str();

		auto root = *layout.document();
		auto output = Unicode::ToUTF8(FlexLayout::Debugger::DumpTree(root));

		ASSERT_STREQ(output.c_str(), expected.c_str());
	}
}
