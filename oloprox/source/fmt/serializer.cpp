#include <glm/glm.hpp>
#include <glm/gtc/packing.hpp>
#include <nlohmann/json.hpp>

namespace nlohmann
{
	template< glm::size_t L, typename T, glm::qualifier Q > void to_json( json &j, const glm::vec< L, T, Q > &v )
	{
		j = json::array( );
		for ( size_t i = 0; i < L; ++i ) j . push_back( v[ i ] );
	}

	template< glm::size_t L, typename T, glm::qualifier Q > void from_json( const json &j, glm::vec< L, T, Q > &v )
	{
		for ( size_t i = 0; i < L; ++i ) v[ i ] = j[ i ] . get< T >( );
	}
}
