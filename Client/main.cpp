#include <algorithm>
#include <ranges>
#include <string>
#include <vector>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <steam/isteamclient.h>
#include <steam/steamtypes.h>

using namespace fmt::literals;
using namespace std;

namespace vs = views;
namespace rng = ranges;

#include <Server.hpp>

namespace fs = boost::filesystem;
namespace dll = boost::dll;

int main( int, char ** )
{

	fmt::print( "vec: {:f}" 
	, 112.8e+55);

	return 0;
}
