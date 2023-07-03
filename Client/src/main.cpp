#include <compare>
#include <exception>
#include <iostream>
import engine.vulkan;

int main( int, char ** )
{
	Renderer renderer;
	try { renderer . run( ); } catch ( std::exception &e ) { std::cerr << e . what( ) << std::endl; }

	return 0;
}
