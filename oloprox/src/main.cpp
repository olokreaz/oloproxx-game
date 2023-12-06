#include <iostream>
#include <string>
#include <vector>

#include <fmt/ranges.h>
#include <spdlog/spdlog.h>

#include <glm/glm.hpp>

import systems;
import types;

int main( int argc, char **argv )
{
	systems::console::Console::initialize( );
	systems::console::Console::setLogLevel( spdlog::level::trace );
	systems::console::Console::setConsoleTitle( "oloprox - terminal" );

	set_default_logger( systems::logging::create_logger( "global" ) );

	return 0;
}

// dxc -spirv -T {lib} -HV 2021 -I {includes} -Fo {output} {input}
// glslc --target-env=vulkan1.3 -fshader-stage={stage} -I {includes} -o {output} {input}
