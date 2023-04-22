/* 
	! Louncher for the game Dooplet
 	TODO: Add a menu to select the friends
 	TODO: Add a menu to select the game mode
 	TODO: Add a menu to select the level
 	TODO: Add a menu to select the number of players
 	TODO: Add a menu to select the settings

	! The game is a 2D platformer
	TODO project: Add Load dll
	TODO project: create vulkan renderer
	TODO project: create save system (save the game)
	TODO project: create load system (load the game)
	TODO project: create a menu settings
	TODO project: create a exit menu
*/

/*
 * deps:
 *	- boost
 *	- spdlog
 *	- yaml-cpp
 *	- vulkan
 *	- glfw
 *	- glm
 *	- shader compiler (glslang)
 *	- steam
 *	- lua
 *	- fmt
 *	- tinyobjloader
 *	- freetype
 *	- discord rpc
 *	- gainput
 *	- discord game sdk
 */

#include <iostream>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32

#include <commandline.h>
#include <sago/platform_folders.h>
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include <boost/filesystem.hpp>
#include <boost/dll/alias.hpp>
#include <boost/dll/config.hpp>
#include <boost/dll/import.hpp>
#include <boost/dll/import_class.hpp>
#include <boost/dll/library_info.hpp>
#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/dll/shared_library.hpp>
#include <boost/dll/smart_library.hpp>

namespace fs = boost::filesystem;

int WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	const fs::path project_dir = fs::path( sago::getConfigHome( ) ) / "oloprox" / "dooplet";
	Commandline    cli;

	return 0;
}
