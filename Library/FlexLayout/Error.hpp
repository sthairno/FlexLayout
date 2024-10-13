#pragma once
#include <Siv3D/Error.hpp>

namespace FlexLayout
{
	class NotFoundError final : public s3d::Error
	{
	public:

		using s3d::Error::Error;

		[[nodiscard]]
		s3d::StringView type() const noexcept override;
	};

	class InvalidTreeOperationError final : public s3d::Error
	{
	public:

		using s3d::Error::Error;

		[[nodiscard]]
		s3d::StringView type() const noexcept override;
	};
}
