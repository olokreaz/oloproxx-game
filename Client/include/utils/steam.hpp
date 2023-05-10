#include <spdlog/spdlog.h>
#include <steam/steam_api.h>

extern std::shared_ptr< spdlog::logger > g_logger;

namespace utils
{
	class Steam final {
		void init( )
		{
			g_logger->info( "Start SteamAPI init" );
			if ( !SteamAPI_Init( ) ) g_logger->error( "Failed SteamAPI init" );
			else g_logger->info( "Finish SteamAPI init" );
		}
		
		void shutdown( )
		{
			g_logger->info( "Shoutdown SteamAPI" );
			SteamAPI_Shutdown( );
		}

	public:
		Steam( ) { init( ); }
		~Steam( ) { shutdown( ); }
	};
}
