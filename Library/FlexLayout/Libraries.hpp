#pragma once
# include <Siv3D/Platform.hpp>

# if !defined(FLEXLAYOUT_TEST) && SIV3D_PLATFORM(WINDOWS)
#	if SIV3D_BUILD(DEBUG)
#		pragma comment (lib, "FlexLayout/FlexLayout_d")
#		pragma comment (lib, "yoga/yogacored")
#	else
#		pragma comment (lib, "FlexLayout/FlexLayout")
#		pragma comment (lib, "yoga/yogacore")
#	endif
# endif
