#pragma once
#include <memory>
#include <string>
#include <boost/any.hpp>

#include <fmt/core.h>

#include <steam/steamtypes.h>

#include "Application/Socket/Client.hpp"
#include "Application/Socket/Server.hpp"

namespace types
{
	class CCommand {
		const std::string          m_command;
		std::shared_ptr< CServer > m_server;
		std::shared_ptr< CClient > m_client;

	public:
		CCommand( const std::string_view command ) : m_command { command } { }
		virtual void execute( boost::any data ) = 0;

		friend bool operator==( const CCommand &lhs, const CCommand &rhs ) { return lhs . m_command == rhs . m_command; }
		friend bool operator!=( const CCommand &lhs, const CCommand &rhs ) { return !( lhs == rhs ); }
		friend bool operator==( const CCommand &lhs, const std::string rhs ) { return lhs . m_command == rhs; }
		friend bool operator!=( const CCommand &lhs, const std::string rhs ) { return !( lhs == rhs ); }
		friend bool operator==( const std::string lhs, const CCommand &rhs ) { return rhs == lhs; }
		friend bool operator!=( const std::string lhs, const CCommand &rhs ) { return !( rhs == lhs ); }
	};
}
