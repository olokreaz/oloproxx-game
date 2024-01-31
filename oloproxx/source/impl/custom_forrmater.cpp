#include <fmt/format.h>
#include <fmt/ostream.h>

#include <glm/glm.hpp>
#include <steam/steamclientpublic.h>

template< typename T , glm::length_t L , glm::qualifier Q > struct fmt::formatter < glm::vec < L, T, Q > >
{
	char separator = ' ';

	constexpr auto parse( format_parse_context &ctx )
	{
		auto it = ctx . begin( ), end = ctx . end( );
		if ( it != end && *it == 'x' )
		{
			++it;
			separator = 'x';
		}
		return it;
	}

	template< typename FormatContext > auto format( const glm::vec < L, T, Q > &v, FormatContext &ctx )
	{
		return format_to( ctx . out( ), "{{{}}}", fmt::join( v, separator ) );
	}
};

// template< > struct fmt::formatter < cx::guid_t > : ostream_formatter { };

template< > struct fmt::formatter < CSteamID >
{
	enum EFMT
	{
		kNull ,
		kNone ,
		ksAccKey ,
		kAccKey ,
		kRender ,
	};

	EFMT mode = kNull;

	constexpr auto parse( format_parse_context &ctx )
	{
		auto it = ctx . begin( ), end = ctx . end( );
		if ( it == end ) return it;

		switch ( *it )
		{
			case 'a' : mode = ksAccKey;
				break;
			case 'k' : mode = kAccKey;
				break;
			case 's' : mode = kRender;
				break;
			case 'i' : mode = kNone;
				break;
			default : return it;
		}
		if ( mode != kNull ) ++it;
		return it;
	}

	template< typename FormatContext > auto format( const CSteamID &id, FormatContext &ctx )
	{
		switch ( mode )
		{
			case ksAccKey : return format_to( ctx . out( ), "{}", id . GetStaticAccountKey( ) );
			case kAccKey : return format_to( ctx . out( ), "{}", id . GetAccountID( ) );
			case kRender : return format_to( ctx . out( ), "{}", id . Render( ) );
			case kNull :
			case kNone :
			default : return format_to( ctx . out( ), "{}", id . ConvertToUint64( ) );
		}
	}
};
