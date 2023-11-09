module;

#include <crossguid/guid.hpp>

export module system.utils;

namespace system::utils
{
	export std::string create_guid( ) { return xg::newGuid( ); }
}
