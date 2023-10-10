module;
#include "utils.hpp"

#include <filesystem>

namespace fs = std::filesystem;

export module HandlersImpl;

export class BinaryHandler : public help::IObserver
{
protected:
	BinaryHandler( ) : IObserver { g_config . source / "", g_config . destination / "" } { }

	void _CopyFile( const fs::path &src, const fs::path &dst ) override { }
	void _CopyDir( const fs::path &src, const fs::path &dst ) override { }
	void _RemoveFile( const fs::path &src, const fs::path &dst ) override { }
	void _RemoveDir( const fs::path &src, const fs::path &dst ) override { }
	void _update( fs::path &path ) override { }
};
