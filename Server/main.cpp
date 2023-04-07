#include <iostream>
#include <string>
#include <fmt/color.h>
#include <boost/filesystem.hpp>
#include <mfl/layout.hpp>
#include <sago/platform_folders.h>
#include <SimpleIni.h>

namespace fs = boost::filesystem;

int main(int, char**)
{

		fs::path project_file = fs::path( sago::getConfigHome( ) ) / "Dooplet"; // Get the %appdata%/project.name path
		fs::create_directories( project_file ); // Create the directory if it doesn't exist

		
		
		CSimpleIniA ini;
		int error = ini.LoadFile( (project_file / "settings.ini").c_str(  ));
		if (error != 0) {/* error handler */}
		

		
		
		
		
		
		
    return 0;
}
