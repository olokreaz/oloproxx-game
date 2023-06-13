#pragma once
#include "types/Command.hpp"

namespace Command::application
{
	class QuitCommand : public types::CCommand {
		void run( )
		{
			spdlog::info( "QuitCommand::run" );
			m_server -> stop( );
		}

	public:
		CApplicationCommand( ) : types::CCommand { "quit" } { }
		void execute( boost::any data ) override {}
	};
}
