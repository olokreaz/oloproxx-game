module;
#include <memory>
#include <stdexcept>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <steam/steam_api.h>
export module steam;

namespace steam
{
	class CSteamApp
	{
		static std::shared_ptr< CSteamApp > m_pInstance     = nullptr;
		bool                                m_bIsInitialize = false;

	protected:
		static auto _log = spdlog::stdout_color_mt( "steam" );
		static auto _socket_log = spdlog::stdout_color_mt( "socket" );

	public:
		CSteamApp( ) = default;

		static std::shared_ptr< CSteamApp > instance( )
		{
			if ( !m_pInstance ) m_pInstance = std::make_shared< CSteamApp >( );
			return m_pInstance;
		}

		void initialize( )
		{
			if ( !m_bIsInitialize )
			{
				if ( !SteamAPI_Init( ) ) throw std::runtime_error( "SteamAPI_Init failed" );
				spdlog::info( "SteamAPI_Init success" );
			} else spdlog::warn( "SteamAPI_Init already initialized" );
		}

		~CSteamApp( ) { SteamAPI_Shutdown( ); }
	};
}
