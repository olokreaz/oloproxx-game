module;

#include <crossguid/guid.hpp>

export module systems.utils;

export namespace systems::utils
{
	std::string create_guid( ) { return xg::newGuid( ); }
}
