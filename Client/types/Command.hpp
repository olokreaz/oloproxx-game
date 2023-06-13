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
		const std::string                 m_command;
		static std::shared_ptr< CServer > m_server;
		static std::shared_ptr< CClient > m_client;

	protected:
		std::shared_ptr< CServer > Server( ) { return !m_server ? nullptr : m_server; }
		std::shared_ptr< CClient > Client( ) { return !m_client ? nullptr : m_client; }

	public:
		CCommand( const std::string_view command ) : m_command { command } { }
		virtual void execute( boost::any data ) = 0;

		friend bool operator==( const CCommand &lhs, const CCommand &rhs ) { return lhs . m_command == rhs . m_command; }
		friend bool operator!=( const CCommand &lhs, const CCommand &rhs ) { return !( lhs == rhs ); }
		friend bool operator==( const CCommand &lhs, const std::string rhs ) { return lhs . m_command == rhs; }
		friend bool operator!=( const CCommand &lhs, const std::string rhs ) { return !( lhs == rhs ); }
		friend bool operator==( const std::string lhs, const CCommand &rhs ) { return rhs == lhs; }
		friend bool operator!=( const std::string lhs, const CCommand &rhs ) { return !( rhs == lhs ); }

		static void setServer( std::shared_ptr< CServer > server ) { m_server = server; }
		static void setClient( std::shared_ptr< CClient > client ) { m_client = client; }
	};

	class CComandHandler {
		friend class CCommand;
		static std::vector< std::shared_ptr< CCommand > > m_commands;

		template< class... Args > void execute( );

	public:
		CComandHandler( )  = default;
		~CComandHandler( ) = default;
	};
}
