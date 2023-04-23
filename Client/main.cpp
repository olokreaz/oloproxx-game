#include <algorithm>
#include <ranges>
#include <string>
#include <vector>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>

using namespace fmt::literals;
using namespace std;

namespace vs = views;
namespace rng = ranges;

int main( int, char ** )
{

	vector< string > names = { "John", "Jane", "Jack", "Jill" };

	auto names_view = names | vs::transform( [] ( const auto &name ) { return name + " Doe"; } );

	fmt::print( "Names: {}\n", names_view );

	return 0;
}

