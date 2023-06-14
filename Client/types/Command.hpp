#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <boost/any.hpp>

#include <fmt/core.h>

#include <steam/steamtypes.h>

#include "Application/Socket/Client.hpp"
#include "Application/Socket/Server.hpp"

using namespace std;

namespace types
{
	template< class... Args > class CArgs {
		std::tuple< Args... > m_args;

	public:
		string operator[]( uint16 index ) { std::get< index >( m_args ); }

		CArgs( )  = default;
		~CArgs( ) = default;
	};

	class CCommand {
		const string m_name;
		const string m_usage;
	};
}
