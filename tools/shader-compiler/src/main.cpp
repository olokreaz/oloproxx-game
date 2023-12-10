#include <filesystem>
#include <fstream>
#include <ranges>
#include <span>
#include <unordered_map>
#include <vector>

#include <glaze/glaze.hpp>

#include <boost/process.hpp>

#include <glob/glob.hpp>

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <magic_enum.hpp>

#include <shader-compiler/config>

namespace fs = std::filesystem;
namespace bp = boost::process;
using namespace std::literals;

// format for the shader compile command

std::unordered_map< std::string, std::string > GlslStageToDxcStage = {
				{ "vert", "vs" },
				{ "tesc", "hs" },
				{ "tese", "ds" },
				{ "geom", "gs" },
				{ "frag", "ps" },
				{ "comp", "cs" }
		};

std::string generateCommand( const std::string &extension, const std::string &shader_stage, const std::string &io_path )
{
	std::string cmd;
	if ( extension == "glsl" ) cmd = fmt::format( GLSLC_COMPILE_FORMAT, shader_stage, "./include/glsl", io_path );
	else if ( extension == "hlsl" ) cmd = fmt::format( DXC_COMPILE_FORMAT, shader_stage, "./include/hlsl", io_path );
	return cmd;
}

void checkCompilationSuccess( size_t hash, const std::string &io_path )
{
	auto nhash = fs::last_write_time( fmt::format( "{}.spv", io_path ) ) . time_since_epoch( ) . count( );
	if ( hash == 0 && nhash != 0 ) fmt::println( "Compile succeeded: {}", io_path );
	else if ( hash != 0 && hash != nhash ) fmt::println( "Compile succeeded: {}", io_path );
	else fmt::println( "Compile Failed: {}", io_path );
}

void compileShader( const fs::path &file )
{
	const auto base_path    = file . parent_path( );
	const auto filename     = file . stem( ) . stem( ) . filename( ) . string( );
	const auto extension    = file . extension( ) . string( ) . substr( 1 );
	const auto shader_stage = file . stem( ) . extension( ) . string( ) . substr( 1 );
	auto       io_path      = fmt::format( "{}/{}.{}", base_path . string( ), filename, shader_stage );

	std::string cmd = generateCommand( extension, shader_stage, io_path );
	if ( cmd . empty( ) ) return;

	fmt::println( "Command: {}", cmd );
	size_t hash = fs::exists( fmt::format( "{}.spv", io_path ) )
				? fs::last_write_time( fmt::format( "{}.spv", io_path ) ) . time_since_epoch( ) . count( )
				: 0;
	bp::ipstream out;
	bp::child    c( cmd, bp::std_out > out );
	std::string  line;
	while ( c . running( ) && std::getline( out, line ) && !line . empty( ) ) fmt::println( "{}", line );
	c . wait( );

	checkCompilationSuccess( hash, io_path );
}

class CFileDiffer
{
	static inline constexpr std::string_view s_filename = "fileTimeStamp.json"sv;

	std::unordered_map< std::string, size_t > m_fileTimeStamps;
	std::unordered_map< std::string, size_t > m_fileTimeStampsOld;
	std::vector< std::string >                m_diff;

	fs::path m_path;

	using iterator = std::vector< std::string >::iterator;
	using const_iterator = std::vector< std::string >::const_iterator;

public:
	CFileDiffer( fs::path path ) : m_path( std::move( path ) )
	{
		try
		{
			createFileTimeStamps( );
			loadFileTimeStampsData( );
			createDiff( );
		} catch ( const std::exception &e ) { handleError( e ); }
	}

	~CFileDiffer( ) { try { save( ); } catch ( const std::exception &e ) { handleError( e ); } }

	const std::vector< std::string >& diff( ) const { return m_diff; }

	iterator begin( ) { return m_diff . begin( ); }
	iterator end( ) { return m_diff . end( ); }

	const_iterator cbegin( ) { return m_diff . cbegin( ); }
	const_iterator cend( ) { return m_diff . cend( ); }

private:
	void loadFileTimeStampsData( )
	{
		if ( !exists( m_path / s_filename ) )
		{
			m_diff . reserve( m_fileTimeStamps . size( ) );
			return;
		}
		load( );
	}

	void handleError( const std::exception &e ) { fmt::println( "error: {}", e . what( ) ); }

	void createDiff( )
	{
		if ( !m_fileTimeStampsOld . size( ) )
		{
			m_diff . reserve( m_fileTimeStamps . size( ) );
			return;
		}
		for ( const auto &[ filename, timestamp ] : m_fileTimeStamps )
			for ( const auto &[ filename_o, timestamp_o ] : m_fileTimeStampsOld )
				if ( filename == filename_o )
					if ( timestamp != timestamp_o ) m_diff . push_back( filename );
					else if ( !fs::exists( fs::path( filename ) . stem( ) . string( ) + ".spv" ) ) m_diff . push_back( filename );
	}

	void createFileTimeStamps( )
	{
		for ( const auto &file : fs::recursive_directory_iterator( m_path ) )
		{
			if ( const auto &ext = file . path( ) . extension( ); ext != ".glsl" && ext != ".hlsl" ) continue;
			const auto hash = last_write_time( file ) . time_since_epoch( ) . count( );
			fmt::println( "file: {} hash: {}", file . path( ) . string( ), hash );
			m_fileTimeStamps[ file . path( ) . string( ) ] = hash;
		}
	}

	void load( )
	{
		auto err = glz::read_file( m_fileTimeStampsOld, ( m_path / s_filename ) . string( ), std::string { } );
		if ( err ) fmt::println( "error: {}", magic_enum::enum_name( err . ec ) );
		else fmt::println( "loaded: {}", m_fileTimeStampsOld );
	}

	void save( )
	{
		auto err = glz::write_file( m_fileTimeStamps, ( m_path / s_filename ) . string( ), std::string { } );
		if ( err ) fmt::println( "error: {}", magic_enum::enum_name( err . ec ) );
		else fmt::println( "saved: {}", m_fileTimeStamps );
	}
};

int main( int argc, char **argv )
{
	const std::span args( argv, argv + argc );
	fs::path        rootDir = args . size( ) > 1 ? fs::path( args[ 1 ] ) : fs::current_path( );

	CFileDiffer differ( rootDir );

	for ( auto &p : differ ) compileShader( p );

	return 0;
}
