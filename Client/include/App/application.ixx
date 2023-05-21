module;
#include <memory>
#include <steam/steamtypes.h>
export module Application;

namespace App
{
	class Application {
		enum class EModeRun {
			none = -1
			, all
			, client
			, server
			,
		};

		class CApp {
			void SteamInit( );
			void SteamShutdown( );

			void     update( );
			EModeRun m_eMode;

			void recv( );
			void send( );
			void handler( );

		public:
			int8 run( );

			EModeRun mode( ) { return m_eMode; }
			auto     mode( EModeRun m ) { m_eMode = m; }

			CApp( );
			~CApp( );

			static inline bool s_bQuit = { false };
		};

		using CApp_t = std::shared_ptr< CApp >;
	};
}
