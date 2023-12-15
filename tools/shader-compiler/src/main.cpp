#include <magic_enum.hpp>

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
	static inline std::unordered_map< std::string, std::pair< size_t, size_t > > s_CompiledFiles;
	static inline std::unordered_set< std::string >
	s_changedFiles;
	static inline fs::path s_directoryPath;

	ShaderCompilerController( ) = delete;

public:
	static void generate( const fs::path &directoryPath )
	{
		s_directoryPath = directoryPath;

		auto filter = [] ( const fs::path &path ) { return is_regular_file( path ) && ( path . extension( ) == ".glsl" || path . extension( ) == ".hlsl" ); };

		for ( const auto &entry : fs::recursive_directory_iterator( directoryPath )
			| std::ranges::views::filter( filter )
			| std::ranges::views::transform( [] ( const auto &entry ) { return entry . path( ); } ) )
			s_CompiledFiles[ entry . string( ) ] = { last_write_time( entry ), 0 };

		try
		{
			if ( exists( s_directoryPath / "compiled-files.json" ) )
			{
				glz::parse_error parErr = glz::read_file_json(
										s_CompiledFiles,
										( s_directoryPath / "compiled-files.json" ) . string( ),
										std::string { }
										);
				if ( parErr != glz::error_code::none ) fmt::print( stderr, "Glaze parse error: {}\n", magic_enum::enum_name( parErr . ec ) );
				for ( auto &[ path, times ] : s_CompiledFiles )
				{
					times . second = times . first;
					times . first  = fs::last_write_time( path );
				}
			}

			s_changedFiles . reserve( s_CompiledFiles . size( ) );
			for ( const auto &[ path, times ] : s_CompiledFiles ) if ( times . first != times . second ) s_changedFiles . insert( path );

			glz::write_error wrErr = glz::write_file_json(
									s_CompiledFiles,
									( s_directoryPath / "compiled-files.json" ) . string( ),
									std::string { }
									);

			if ( wrErr != glz::error_code::none ) fmt::print( stderr, "Glaze write error: {}\n", magic_enum::enum_name( wrErr . ec ) );
		} catch ( const std::exception &e ) { fmt::print( stderr, "An error occurred while reading or writing files: {}\n", e . what( ) ); }
	}

	static void compile( )
	{
		using namespace std::string_view_literals;
		static const std::string_view outputExtension = ".spv"sv;
		for ( const auto &entry : s_changedFiles | std::ranges::views::transform( [] ( const auto &path ) { return fs::path( path ); } ) )
			if ( is_regular_file( entry ) && ( entry . extension( ) == ".glsl" || entry . extension( ) == ".hlsl" ) )
			{
				fs::path outputPath = entry;

				outputPath . replace_extension( outputExtension );

				std::string command = fmt::format( kShaderFormantCompileString, outputPath . string( ), entry . string( ) );

				fmt::println( "cmd: {}", command );

				bp::ipstream output;

				auto c = bp::child(
						command,
						bp::start_dir = entry . parent_path( ),
						bp::std_out > output,
						bp::std_err > output
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

int main( int argc, char **argv )
{
	try
	{
		if ( argc > 1 ) ShaderCompilerController::generate( argv[ 1 ] );
		else ShaderCompilerController::generate( fs::current_path( ) );
		ShaderCompilerController::compile( );
		fmt::print( "Shader compilation completed successfully!\n" );
	} catch ( const std::exception &e ) { fmt::print( stderr, "An error occurred: {}\n", e . what( ) ); }
}
