module;
#include <string>
#include <vcruntime_typeinfo.h>

#include <steam/isteamnetworkingsockets.h>
#include <steam/steamtypes.h>
using namespace std;
export module types;

namespace types
{
	export template< class TyObj > class IMessage {
	public:
		const uint32 m_size;
		const string m_Type;

		IMessage( ): m_size { sizeof TyObj },
			     m_Type { typeid( TyObj ).name( ) } {}

		~IMessage( ) = default;
	};
}

class CMSG : public types::IMessage< CMSG > {
	CSteamID m_steamID;
	string   m_msg;

public:
	CMSG( ) {}
	~CMSG( ) = default;
};
