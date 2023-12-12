#include <shader-compiler/config>

#include <ranges>
#include <unordered_map>
#include <unordered_set>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <fmt/core.h>

#include <glaze/glaze.hpp>

namespace fs = boost::filesystem;
namespace bp = boost::process;

class ShaderCompilerController
{
	static inline std::unordered_map< std::string, size_t > s_CompiledFiles;
	static inline std::unordered_map< std::string, size_t > s_CompiledFilesOld;
	static inline std::unordered_set< std::string >
	s_changedFiles;
	static inline fs::path s_directoryPath;

public:
	static void generate( const fs::path &directoryPath )
	{
		s_directoryPath = directoryPath;

		auto fillter = [] ( const fs::path &path ) { return is_regular_file( path ) && ( path . extension( ) == ".glsl" || path . extension( ) == ".hlsl" ); };

		for (
			const auto &entry
			: fs::recursive_directory_iterator( directoryPath )
			| std::ranges::views::filter( fillter )
			| std::ranges::views::transform( [] ( const auto &entry ) { return entry . path( ); } )
		)
			s_CompiledFiles[ entry . string( ) ] = fs::last_write_time( entry );

		if ( fs::exists( s_directoryPath / "compiled-files.json" ) )
			auto _ = glz::read_file_json(
							s_CompiledFilesOld,
							( s_directoryPath / "compiled-files.json" ) . string( ),
							std::string { }
						);

		// s_CompiledFilesOld == 0
		s_changedFiles . reserve( s_CompiledFiles . size( ) );
		if ( s_CompiledFilesOld . empty( ) ) s_changedFiles . insert_range( s_CompiledFiles | std::ranges::views::keys );

		// s_CompiledFilesOld > 0
		for ( const auto &[ path, hash ] : s_CompiledFiles )
			if (
				auto it = s_CompiledFilesOld . find( path ); it == s_CompiledFilesOld . end( )
				|| s_CompiledFilesOld[ path ] != hash
			)
				s_changedFiles . insert( path );
			else if ( s_CompiledFilesOld[ path ] == hash && !fs::exists( fs::path( path ) . replace_extension( "spv" ) ) ) s_changedFiles . insert( path );

		auto _ = glz::write_file_json(
						s_CompiledFiles,
						( s_directoryPath / "compiled-files.json" ) . string( ),
						std::string { }
						);
	}

	static void compile( )
	{
		const fs::path outputExtension = ".spv";

		for ( const auto &entry : s_changedFiles | std::ranges::views::transform( [] ( const auto &path ) { return fs::path( path ); } ) )
			if ( is_regular_file( entry ) && ( entry . extension( ) == ".glsl" || entry . extension( ) == ".hlsl" ) )
			{
				fs::path outputPath = entry;
				outputPath . replace_extension( outputExtension );

				std::string command = fmt::format( COMPILE_STRING_FORMAT, outputPath . string( ), entry . string( ) );

				fmt::println( "cmd: {}", command );

				bp::ipstream output;

				auto c = bp::child(
						command,
						bp::start_dir = entry . parent_path( ),
						bp::std_out > output
						);

				std::string line;
				while ( std::getline( output, line ) ) fmt::print( "{}", line );

				c . wait( );
				if ( c . exit_code( ) )
					fmt::println(
							"An error occurred while compiling shader: {}",
							entry . string( )
						);
			}
	}
};

int main( int argc, char *argv[ ] )
{
	try
	{
		if ( argc > 1 ) ShaderCompilerController::generate( argv[ 1 ] );
		else ShaderCompilerController::generate( fs::current_path( ) );
		ShaderCompilerController::compile( );
		fmt::print( "Shader compilation completed successfully!\n" );
	} catch ( const std::exception &e ) { fmt::print( stderr, "An error occurred: {}\n", e . what( ) ); }
}
