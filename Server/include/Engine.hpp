#pragma once
#include "../Server.hpp"

#include <spdlog/spdlog.h>

namespace server
{
	class CEngine : public IServer {
		spdlog::logger m_logger;
		CEngine( );

	public:
		~CEngine( ) override;

		void init( ) override;
		void send( ) override;
		void recv( ) override;
		void update( ) override;

		static boost::shared_ptr< CEngine > create( )
		{
			auto ptr = boost::shared_ptr< CEngine >( new CEngine );
			ptr->m_logger.info( "Create Server Engine module" );
			return ptr;
		}
	};

}
