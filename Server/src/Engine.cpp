#include "Engine.hpp"

#include <commandline.h>
#include <boost/filesystem/path.hpp>
#include <sago/platform_folders.h>

#include <windows.h>
#include <yaml-cpp/yaml.h>

namespace fs = boost::filesystem;

#include <utils.hpp>

server::CEngine::CEngine( ) :
	m_logger { "Server" }
{
	{
		const fs::path project_dir = fs::path( ) / "oloprox" / "dooplet";
		Commandline    cli;

		utils::LoadGeneralConfig( project_dir, cli );

		YAML::Node config;
		utils::file( project_dir / "config.yaml" ) >> config;
		utils::GenereteConfig( project_dir, cli, config );

	}
}

server::CEngine::~CEngine( ) {}
void server::CEngine::init( ) {}
void server::CEngine::send( ) {}
void server::CEngine::recv( ) {}
void server::CEngine::update( ) {}

BOOST_DLL_ALIAS( server::CEngine::create, create_server )

int DllMain( HINSTANCE hInstance, DWORD fdwReason, LPVOID lpReserved )
{

	switch ( fdwReason ) {
		case DLL_PROCESS_ATTACH :
			// Initialize once for each new process.
			// Return FALSE to fail DLL load.
			fmt::print( "DLL PROCESS ATTACH\n" );
			break;

		case DLL_THREAD_ATTACH :
			// Do thread-specific initialization.
			fmt::print( "DLL THREAD ATTACH\n" );
			break;

		case DLL_THREAD_DETACH :
			// Do thread-specific cleanup.
			fmt::print( "DLL THREAD DETACH\n" );
			break;

		case DLL_PROCESS_DETACH :
			// Perform any necessary cleanup.
			fmt::print( "DLL PROCESS DETACH\n" );
			break;
	}

	return 1;
}
