#include <fmt/format.h>
#include <glm/glm.hpp>

template< typename T, glm::length_t L, glm::qualifier Q > struct fmt::formatter< glm::vec< L, T, Q > >
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

	template< typename FormatContext > auto format( const glm::vec< L, T, Q > &v, FormatContext &ctx )
	{
		return format_to( ctx . out( ), "({})", fmt::join( v, separator ) );
	}
};
