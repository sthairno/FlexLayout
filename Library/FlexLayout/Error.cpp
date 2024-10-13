#include "Error.hpp"

namespace FlexLayout
{
	s3d::StringView NotFoundError::type() const noexcept
	{
		return U"FlexLayout::NotFoundError";
	}

	s3d::StringView InvalidTreeOperationError::type() const noexcept
	{
		return U"FlexLayout::InvalidTreeOperationError";
	}
}
