#include <algorithm>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <steam/isteamclient.h>
#include <steam/steamtypes.h>

#include <boost/function.hpp>

using namespace fmt::literals;
using namespace std;

namespace vs = views;
namespace rng = ranges;

class Steam final {
	void init( );
	void shutdown( );

	string operator[]( string_view key ) const;

};

#include <discord_rpc.h>

static void UpdatePresence( )
{
	DiscordRichPresence discordPresence;
	memset( &discordPresence, 0, sizeof( discordPresence ) );
	discordPresence.state          = "Playing Solo";
	discordPresence.details        = "Competitive";
	discordPresence.startTimestamp = 1507665886;
	discordPresence.endTimestamp   = 1507665886;
	discordPresence.largeImageText = "Numbani";
	discordPresence.smallImageText = "Rogue - Level 100";
	discordPresence.partyId        = "ae488379-351d-4a4f-ad32-2b9b01c91657";
	discordPresence.partySize      = 1;
	discordPresence.partyMax       = 5;
	discordPresence.joinSecret     = "MTI4NzM0OjFpMmhuZToxMjMxMjM= ";
	Discord_UpdatePresence( &discordPresence );
}

int main( int, char ** )
{

	string str;

	while ( true )
		if ( getline( cin, str ) ) break;
		else UpdatePresence( );

	return 0;
}
