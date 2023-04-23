#define BUILD_DLL
#include <.h>

#include <boost/dll.hpp>
namespace dll = boost::dll;

namespace server
{

	class API IServer {

	public:
		virtual void init( ) = 0;
		virtual void send( ) = 0;
		virtual void recv( ) = 0;
		virtual void update( ) = 0;

		virtual ~IServer( ) {};

		void run( )
		{
			init( );
			while ( true ) {
				send( );
				recv( );
				update( );
			}
		}
	};

}
