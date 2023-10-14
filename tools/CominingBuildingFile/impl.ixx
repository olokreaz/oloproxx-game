module;
#include "utils.hpp"

#include <filesystem>
#include <memory>

namespace fs = std::filesystem;

export module HandlersImpl;

export class BinaryHandler : public help::IObserver
{
	BinaryHandler( ) : IObserver { g_config . source / "bin", g_config . destination / "" } {}

public:
	static std::unique_ptr< help::IObserver > create( ) { return std::unique_ptr< IObserver >( new BinaryHandler ); }

	void _CopyFile( const fs::path &src, const fs::path &dst ) override {}
	void _CopyDir( const fs::path &src, const fs::path &dst ) override {}
	void _RemoveFile( const fs::path &src, const fs::path &dst ) override {}
	void _RemoveDir( const fs::path &src, const fs::path &dst ) override {}
	void _update( fs::path &path ) override {}
};
