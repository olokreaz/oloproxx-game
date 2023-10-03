module;
#include <memory>
#include <stdexcept>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <steam/steam_api.h>
export module steam;

namespace steam
{
	class CSteamAppInterface
	{
		inline static std::shared_ptr< CSteamAppInterface > s_pInstance     = nullptr;
		bool                                                m_bIsInitialize = false;

	protected:
		inline static auto _log = spdlog::stdout_color_mt( "steam" );

		CSteamAppInterface( ) { atexit( []( ) { SteamAPI_Shutdown( ); } ); }

	public:
		static std::shared_ptr< CSteamAppInterface > instance( )
		{
			if ( !s_pInstance ) s_pInstance = std::shared_ptr< CSteamAppInterface >( new CSteamAppInterface );
			return s_pInstance;
		}

		void initialize( )
		{
			if ( !s_pInstance ) throw std::runtime_error( "CSteamAppInterface not initialized" );
			if ( !m_bIsInitialize )
			{
				if ( !SteamAPI_Init( ) ) throw std::runtime_error( "SteamAPI_Init failed" );
				spdlog::info( "SteamAPI_Init success" );
			} else spdlog::warn( "SteamAPI_Init already initialized" );
		}

		~CSteamAppInterface( ) { SteamAPI_Shutdown( ); }
	};
}
