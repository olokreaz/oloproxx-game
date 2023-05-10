module;
#include <string>
#include <steam/steamtypes.h>
using namespace std;
export module types;

namespace types
{
	export template< class TyObj > class CMessage {
		uint32 m_size;
		string m_Type;
	public:
		CMessage( ) {}
		~CMessage( ) {}
	};
}
